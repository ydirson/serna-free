// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "Style.h"
#include "VM.h"
#include "Interpreter.h"
#include "InterpreterMessages.h"
#include "SosofoObj.h"
#include "macros.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

StyleStack::StyleStack()
: level_(0)
{
}

void StyleStack::pushContinue(StyleObj *style,
			      const ProcessingMode::Rule *rule,
			      const NodePtr &nodePtr,
			      Messenger *mgr)
{
  StyleObjIter iter;
  style->appendIter(iter);
  for (;;) {
    const VarStyleObj *varStyle;
    ConstPtr<InheritedC> spec(iter.next(varStyle));
    if (spec.isNull())
      break;
    size_t ind = spec->index();
    if (ind >= inheritedCInfo_.size())
      inheritedCInfo_.resize(ind + 1);
    Ptr<InheritedCInfo> &info = inheritedCInfo_[ind];
    if (!info.isNull() && info->valLevel == level_) {
      if (rule) {
	ASSERT(info->rule != 0);
	if (rule->compareSpecificity(*info->rule) == 0) {
	  mgr->setNextLocation(info->rule->location());
	  mgr->message(InterpreterMessages::ambiguousStyle,
	               StringMessageArg(info->spec->identifier()->name()),
		       rule->location());
	}
      }

    }
    else {
      popList_->list.push_back(ind);
      info = new InheritedCInfo(spec, varStyle, level_, level_, rule, info);
    }
  }
}

void StyleStack::pushEnd(VM &vm, FOTBuilder &fotb)
{
  const PopList *oldPopList = popList_->prev.pointer();
  if (oldPopList) {
    for (size_t i = 0; i < oldPopList->dependingList.size(); i++) {
      size_t d = oldPopList->dependingList[i];
      // d is the index of a characteristic that depends on the actual
      // value of another characteritistic
      if (inheritedCInfo_[d]->valLevel != level_) {
	const Vector<size_t> &dependencies = inheritedCInfo_[d]->dependencies;
	bool changed = 0;
	for (size_t j = 0; j < dependencies.size(); j++) {
	  const InheritedCInfo *p = inheritedCInfo_[dependencies[j]].pointer();
	  if (p && p->valLevel == level_) {
	    inheritedCInfo_[d] = new InheritedCInfo(inheritedCInfo_[d]->spec,
						    inheritedCInfo_[d]->style,
						    level_,
						    inheritedCInfo_[d]->specLevel,
						    inheritedCInfo_[d]->rule,
	                                            inheritedCInfo_[d]);
	    popList_->list.push_back(d);
	    changed = 1;
	    break;
	  }
	}
	// If it changed, then doing set() on the new value will add
	// it to the dependingList for this level.
	if (!changed)
	  popList_->dependingList.push_back(d);
      }
    }
  }
  vm.styleStack = this;
  for (size_t i = 0; i < popList_->list.size(); i++) {
    InheritedCInfo &info = *inheritedCInfo_[popList_->list[i]];
    vm.specLevel = info.specLevel;
    info.spec->set(vm, info.style, fotb, info.cachedValue, info.dependencies);
    if (info.dependencies.size())
      popList_->dependingList.push_back(popList_->list[i]);
  }
  vm.styleStack = 0;
}

void StyleStack::pop()
{
  for (size_t i = 0; i < popList_->list.size(); i++) {
    size_t ind = popList_->list[i];
    ASSERT(inheritedCInfo_[ind]->valLevel == level_);
    Ptr<InheritedCInfo> tem(inheritedCInfo_[ind]->prev);
    inheritedCInfo_[ind] = tem;
  }
  level_--;
  Ptr<PopList> tem(popList_->prev);
  popList_ = tem;
}

ELObj *StyleStack::inherited(const ConstPtr<InheritedC> &ic, unsigned specLevel,
			     Interpreter &interp, Vector<size_t> &dependencies)
{
  ASSERT(specLevel != unsigned(-1));
  size_t ind = ic->index();
  ConstPtr<InheritedC> spec;
  const VarStyleObj *style = 0;
  unsigned newSpecLevel = unsigned(-1);
  if (ind >= inheritedCInfo_.size())
    spec = ic;
  else {
    const InheritedCInfo *p = inheritedCInfo_[ind].pointer();
    while (p != 0) {
      if (p->specLevel < specLevel)
	break;
      p = p->prev.pointer();
    }
    if (!p)
      spec = ic;
    else {
      if (p->cachedValue) {
	// We can only use the cached value if none of the values
	// we depended on changed since we computed it.
	bool cacheOk = 1;
	for (size_t i = 0; i < p->dependencies.size(); i++) {
	  size_t d = p->dependencies[i];
	  if (d < inheritedCInfo_.size()
	      && inheritedCInfo_[d]->valLevel > p->valLevel) {
	    cacheOk = 0;
	    break;
	  }
	}
	if (cacheOk)
	  return p->cachedValue;
      }
      style = p->style;
      spec = p->spec;
      newSpecLevel = p->specLevel;
    }
  }
  VM vm(interp);
  vm.styleStack = this;
  vm.specLevel = newSpecLevel;
  return spec->value(vm, style, dependencies);
}

ELObj *StyleStack::actual(const ConstPtr<InheritedC> &ic, const Location &loc,
			  Interpreter &interp, Vector<size_t> &dependencies)
{
  size_t ind = ic->index();
  for (size_t i = 0; i < dependencies.size(); i++) {
    if (dependencies[i] == ind) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::actualLoop,
		     StringMessageArg(ic->identifier()->name()));
      return interp.makeError();
    }
  }
  dependencies.push_back(ind);
  ConstPtr<InheritedC> spec;
  const VarStyleObj *style = 0;
  if (ind >= inheritedCInfo_.size())
    spec = ic;
  else {
    const InheritedCInfo *p = inheritedCInfo_[ind].pointer();
    if (!p)
      spec = ic;
    else if (p->cachedValue) {
      const Vector<size_t> &dep = p->dependencies;
      for (size_t i = 0; i < dep.size(); i++)
	dependencies.push_back(dep[i]);
      return p->cachedValue;
    }
    else {
      style = p->style;
      spec = p->spec;
    }
  }
  VM vm(interp);
  vm.styleStack = this;
  vm.specLevel = level_;
  return spec->value(vm, style, dependencies);
}

void StyleStack::trace(Collector &c) const
{
  for (size_t i = 0; i < inheritedCInfo_.size(); i++) {
    for (const InheritedCInfo *p = inheritedCInfo_[i].pointer();
         p;
	 p = p->prev.pointer()) {
      c.trace(p->style);
      c.trace(p->cachedValue);
    }
  }
}

InheritedCInfo::InheritedCInfo(const ConstPtr<InheritedC> &sp,
			       const VarStyleObj *so,
			       unsigned vl,
			       unsigned sl,
			       const ProcessingMode::Rule *r,
			       const Ptr<InheritedCInfo> &p)
: spec(sp), style(so), valLevel(vl), specLevel(sl), rule(r), prev(p), cachedValue(0)
{
}

StyleObj *StyleObj::asStyle()
{
  return this;
}

VarStyleObj::VarStyleObj(const ConstPtr<StyleSpec> &styleSpec, StyleObj *use, ELObj **display,
			 const NodePtr &node)
: styleSpec_(styleSpec), use_(use), display_(display), node_(node)
{
  hasSubObjects_ = 1;
}

VarStyleObj::~VarStyleObj()
{
  delete [] display_;
}

void VarStyleObj::traceSubObjects(Collector &c) const
{
  c.trace(use_);
  if (display_)
    for (ELObj **pp = display_; *pp; pp++)
      c.trace(*pp);
}

void VarStyleObj::appendIterForce(StyleObjIter &iter) const
{
  if (styleSpec_->forceSpecs.size())
    iter.append(&styleSpec_->forceSpecs, this);
}

void VarStyleObj::appendIterNormal(StyleObjIter &iter) const
{
  if (styleSpec_->specs.size())
    iter.append(&styleSpec_->specs, this);
  if (use_)
    use_->appendIter(iter);
}

void VarStyleObj::appendIter(StyleObjIter &iter) const
{
  VarStyleObj::appendIterForce(iter);
  VarStyleObj::appendIterNormal(iter);
}

OverriddenStyleObj::OverriddenStyleObj(BasicStyleObj *basic, StyleObj *override)
: basic_(basic), override_(override)
{
  hasSubObjects_ = 1;
}

void OverriddenStyleObj::traceSubObjects(Collector &c) const
{
  c.trace(basic_);
  c.trace(override_);
}

void OverriddenStyleObj::appendIter(StyleObjIter &iter) const
{
  basic_->appendIterForce(iter);
  override_->appendIter(iter);
  basic_->appendIterNormal(iter);
}

MergeStyleObj::MergeStyleObj()
{
  hasSubObjects_ = 1;
}

void MergeStyleObj::append(StyleObj *obj)
{
  styles_.push_back(obj);
}

void MergeStyleObj::appendIter(StyleObjIter &iter) const
{
  for (size_t i = 0; i < styles_.size(); i++)
    styles_[i]->appendIter(iter);
}

void MergeStyleObj::traceSubObjects(Collector &c) const
{
  for (size_t i = 0; i < styles_.size(); i++)
    c.trace(styles_[i]);
}

ColorObj *ColorObj::asColor()
{
  return this;
}

DeviceRGBColorObj::DeviceRGBColorObj(unsigned char red, unsigned char green,
				     unsigned char blue)
{
  color_.red = red;
  color_.green = green;
  color_.blue = blue;
}

void DeviceRGBColorObj::set(FOTBuilder &fotb) const
{
  fotb.setColor(color_);
}

void DeviceRGBColorObj::setBackground(FOTBuilder &fotb) const
{
  fotb.setBackgroundColor(color_);
}

ColorSpaceObj *ColorSpaceObj::asColorSpace()
{
  return this;
}

ELObj *DeviceRGBColorSpaceObj::makeColor(int argc, ELObj **argv,
					 Interpreter &interp, const Location &loc)
{
  if (argc == 0)
    return new (interp) DeviceRGBColorObj(0, 0, 0);
  if (argc != 3) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::RGBColorArgCount);
    return interp.makeError();
  }
  unsigned char c[3];
  for (int i = 0; i < 3; i++) {
    double d;
    if (!argv[i]->realValue(d)) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::RGBColorArgType);
      return interp.makeError();
    }
    if (d < 0.0 || d > 1.0) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::RGBColorArgRange);
      return interp.makeError();
    }
    c[i] = (unsigned char)(d*255.0 + .5);
  }
  return new (interp) DeviceRGBColorObj(c[0], c[1], c[2]);
}

VarInheritedC::VarInheritedC(const ConstPtr<InheritedC> &ic,
			     const InsnPtr &code, const Location &loc)
: InheritedC(ic->identifier(), ic->index()), inheritedC_(ic), code_(code), loc_(loc)
{
}

void VarInheritedC::set(VM &vm, const VarStyleObj *style, FOTBuilder &fotb,
			ELObj *&cacheObj, Vector<size_t> &dependencies) const
{
  if (!cacheObj) {
    EvalContext::CurrentNodeSetter cns(style->node(), 0, vm);
    vm.actualDependencies = &dependencies;
    cacheObj = vm.eval(code_.pointer(), style->display());
    ASSERT(cacheObj != 0);
    vm.actualDependencies = 0;
  }
  if (!vm.interp->isError(cacheObj)) {
    ConstPtr<InheritedC> c(inheritedC_->make(cacheObj, loc_, *vm.interp));
    if (!c.isNull())
      c->set(vm, 0, fotb, cacheObj, dependencies);
  }
}

ConstPtr<InheritedC> VarInheritedC::make(ELObj *obj, const Location &loc, Interpreter &interp)
     const
{
  return inheritedC_->make(obj, loc, interp);
}

ELObj *VarInheritedC::value(VM &vm, const VarStyleObj *style,
			    Vector<size_t> &dependencies) const
{
  EvalContext::CurrentNodeSetter cns(style->node(), 0, vm);
  vm.actualDependencies = &dependencies;
  return vm.eval(code_.pointer(), style->display());
}

StyleObjIter::StyleObjIter()
: i_(0), vi_(0)
{
}

void StyleObjIter::append(const Vector<ConstPtr<InheritedC> > *v, const VarStyleObj *obj)
{
  styleVec_.push_back(obj);
  vecs_.push_back(v);
}

ConstPtr<InheritedC> StyleObjIter::next(const VarStyleObj *&style)
{
  for (; vi_ < vecs_.size(); vi_++, i_ = 0) {
    if (i_ < vecs_[vi_]->size()) {
      style = styleVec_[vi_];
      return (*vecs_[vi_])[i_++];
    }
  }
  return ConstPtr<InheritedC>();
}

StyleSpec::StyleSpec(Vector<ConstPtr<InheritedC> > &fs, Vector<ConstPtr<InheritedC> > &s)
{
  fs.swap(forceSpecs);
  s.swap(specs);
}

#ifdef DSSSL_NAMESPACE
}
#endif

