// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "ProcessContext.h"
#include "Interpreter.h"
#include "InterpreterMessages.h"
#include "SosofoObj.h"
#include "macros.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

FlowObj::FlowObj()
: style_(0)
{
  hasSubObjects_ = 1;
}

void FlowObj::process(ProcessContext &context)
{
  context.startFlowObj();
  unsigned flags = 0;
  pushStyle(context, flags);
  processInner(context);
  popStyle(context, flags);
  context.endFlowObj();
}

void FlowObj::pushStyle(ProcessContext &context, unsigned &)
{
  if (style_)
    context.currentStyleStack().push(style_, context.vm(), context.currentFOTBuilder());
  else
    context.currentStyleStack().pushEmpty();
}

void FlowObj::popStyle(ProcessContext &context, unsigned)
{
  if (style_)
    context.currentStyleStack().pop();
  else
    context.currentStyleStack().popEmpty();
}

void FlowObj::traceSubObjects(Collector &c) const
{
  c.trace(style_);
}

CompoundFlowObj *FlowObj::asCompoundFlowObj()
{
  return 0;
}

bool FlowObj::hasNonInheritedC(const Identifier *) const
{
  return 0;
}

bool FlowObj::hasPseudoNonInheritedC(const Identifier *) const
{
  return 0;
}

void FlowObj::setNonInheritedC(const Identifier *, ELObj *, const Location &, Interpreter &)
{
  CANNOT_HAPPEN();
}

bool FlowObj::isDisplayNIC(const Identifier *ident)
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyPositionPreference:
    case Identifier::keyIsKeepWithPrevious:
    case Identifier::keyIsKeepWithNext:
    case Identifier::keyKeep:
    case Identifier::keyBreakBefore:
    case Identifier::keyBreakAfter:
    case Identifier::keyIsMayViolateKeepBefore:
    case Identifier::keyIsMayViolateKeepAfter:
    case Identifier::keySpaceBefore:
    case Identifier::keySpaceAfter:
      return 1;
    default:
      break;
    }
  }
  return 0;
}

bool FlowObj::setDisplayNIC(FOTBuilder::DisplayNIC &nic,
			    const Identifier *ident, ELObj *obj,
			    const Location &loc, Interpreter &interp)
{
  static FOTBuilder::Symbol breakVals[] = {
    FOTBuilder::symbolFalse,
    FOTBuilder::symbolPage,
    FOTBuilder::symbolColumnSet,
    FOTBuilder::symbolColumn
  };
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyPositionPreference:
      {
	static FOTBuilder::Symbol vals[] = {
	  FOTBuilder::symbolFalse,
	  FOTBuilder::symbolTop,
	  FOTBuilder::symbolBottom,
	};
	interp.convertEnumC(vals, SIZEOF(vals), obj, ident, loc, nic.positionPreference);
      }
      return 1;
    case Identifier::keyIsKeepWithPrevious:
      interp.convertBooleanC(obj, ident, loc, nic.keepWithPrevious);
      return 1;
    case Identifier::keyIsKeepWithNext:
      interp.convertBooleanC(obj, ident, loc, nic.keepWithNext);
      return 1;
    case Identifier::keyKeep:
      {
	static FOTBuilder::Symbol vals[] = {
	  FOTBuilder::symbolFalse,
	  FOTBuilder::symbolTrue,
	  FOTBuilder::symbolPage,
	  FOTBuilder::symbolColumnSet,
	  FOTBuilder::symbolColumn
	};
	interp.convertEnumC(vals, SIZEOF(vals), obj, ident, loc, nic.keep);
      }
      return 1;
    case Identifier::keyBreakBefore:
      interp.convertEnumC(breakVals, SIZEOF(breakVals), obj, ident, loc, nic.breakBefore);
      return 1;
    case Identifier::keyBreakAfter:
       interp.convertEnumC(breakVals, SIZEOF(breakVals), obj, ident, loc, nic.breakAfter);
     return 1;
    case Identifier::keyIsMayViolateKeepBefore:
      interp.convertBooleanC(obj, ident, loc, nic.mayViolateKeepBefore);
      return 1;
    case Identifier::keyIsMayViolateKeepAfter:
      interp.convertBooleanC(obj, ident, loc, nic.mayViolateKeepAfter);
      return 1;
    case Identifier::keySpaceBefore:
    case Identifier::keySpaceAfter:
      {
	FOTBuilder::DisplaySpace &ds = (key == Identifier::keySpaceBefore
				  	? nic.spaceBefore
					: nic.spaceAfter);
	DisplaySpaceObj *dso = obj->asDisplaySpace();
	if (dso)
	  ds = dso->displaySpace();
	else if (interp.convertLengthSpecC(obj, ident, loc, ds.nominal)) {
	  ds.max = ds.nominal;
	  ds.min = ds.nominal;
	}
      }
      return 1;
    default:
      break;
    }
  }
  return 0;
}


void CompoundFlowObj::processInner(ProcessContext &context)
{
  if (content_)
    content_->process(context);
  else
    context.processChildren(context.vm().interp->initialProcessingMode());
}

void CompoundFlowObj::traceSubObjects(Collector &c) const
{
  c.trace(content_);
  FlowObj::traceSubObjects(c);
}

CompoundFlowObj *CompoundFlowObj::asCompoundFlowObj()
{
  return this;
}

class DisplayGroupFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  DisplayGroupFlowObj();
  DisplayGroupFlowObj(const DisplayGroupFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
protected:
  Owner<FOTBuilder::DisplayGroupNIC> nic_;
};

DisplayGroupFlowObj::DisplayGroupFlowObj()
: nic_(new FOTBuilder::DisplayGroupNIC)
{
}

DisplayGroupFlowObj::DisplayGroupFlowObj(const DisplayGroupFlowObj &fo)
: CompoundFlowObj(fo), nic_(new FOTBuilder::DisplayGroupNIC(*fo.nic_))
{
}

void DisplayGroupFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startDisplayGroup(*nic_);
  CompoundFlowObj::processInner(context);
  fotb.endDisplayGroup();
}

bool DisplayGroupFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key) && key == Identifier::keyCoalesceId)
    return 1;
  return isDisplayNIC(ident);
}

void DisplayGroupFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					   const Location &loc, Interpreter &interp)
{
  if (!setDisplayNIC(*nic_, ident, obj, loc, interp)) {
    const Char *s;
    size_t n;
    if (!obj->stringData(s, n)) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidCharacteristicValue,
  		     StringMessageArg(ident->name()));
    }
    nic_->hasCoalesceId = 1;
    nic_->coalesceId.assign(s, n);
  }
}

FlowObj *DisplayGroupFlowObj::copy(Collector &c) const
{
  return new (c) DisplayGroupFlowObj(*this);
}

class ParagraphFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  ParagraphFlowObj();
  ParagraphFlowObj(const ParagraphFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
private:
  Owner<FOTBuilder::ParagraphNIC> nic_;
};


ParagraphFlowObj::ParagraphFlowObj()
: nic_(new FOTBuilder::ParagraphNIC)
{
}

ParagraphFlowObj::ParagraphFlowObj(const ParagraphFlowObj &fo)
: CompoundFlowObj(fo), nic_(new FOTBuilder::ParagraphNIC(*fo.nic_))
{
}

void ParagraphFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startParagraph(*nic_);
  CompoundFlowObj::processInner(context);
  fotb.endParagraph();
}

void ParagraphFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					const Location &loc, Interpreter &interp)
{
  setDisplayNIC(*nic_, ident, obj, loc, interp);
}

bool ParagraphFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  return isDisplayNIC(ident);
}

FlowObj *ParagraphFlowObj::copy(Collector &c) const
{
  return new (c) ParagraphFlowObj(*this);
}

class ParagraphBreakFlowObj : public FlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  ParagraphBreakFlowObj();
  ParagraphBreakFlowObj(const ParagraphBreakFlowObj &);
  FlowObj *copy(Collector &) const;
  void processInner(ProcessContext &);
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
private:
  Owner<FOTBuilder::ParagraphNIC> nic_;
};


ParagraphBreakFlowObj::ParagraphBreakFlowObj()
: nic_(new FOTBuilder::ParagraphNIC)
{
}

ParagraphBreakFlowObj::ParagraphBreakFlowObj(const ParagraphBreakFlowObj &fo)
: FlowObj(fo), nic_(new FOTBuilder::ParagraphNIC(*fo.nic_))
{
}

FlowObj *ParagraphBreakFlowObj::copy(Collector &c) const
{
  return new (c) ParagraphBreakFlowObj(*this);
}

void ParagraphBreakFlowObj::processInner(ProcessContext &context)
{
  context.currentFOTBuilder().paragraphBreak(*nic_);
}

void ParagraphBreakFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					     const Location &loc, Interpreter &interp)
{
  setDisplayNIC(*nic_, ident, obj, loc, interp);
}

bool ParagraphBreakFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  return isDisplayNIC(ident);
}

class ExternalGraphicFlowObj : public FlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  ExternalGraphicFlowObj();
  ExternalGraphicFlowObj(const ExternalGraphicFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
private:
  Owner<FOTBuilder::ExternalGraphicNIC> nic_;
};


ExternalGraphicFlowObj::ExternalGraphicFlowObj()
: nic_(new FOTBuilder::ExternalGraphicNIC)
{
}

ExternalGraphicFlowObj::ExternalGraphicFlowObj(const ExternalGraphicFlowObj &fo)
: FlowObj(fo), nic_(new FOTBuilder::ExternalGraphicNIC(*fo.nic_))
{
}

void ExternalGraphicFlowObj::processInner(ProcessContext &context)
{
  context.currentFOTBuilder().externalGraphic(*nic_);
}

void ExternalGraphicFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					      const Location &loc, Interpreter &interp)
{
  if (!setDisplayNIC(*nic_, ident, obj, loc, interp)) {
    Identifier::SyntacticKey key;
    if (ident->syntacticKey(key)) {
      switch (key) {
      case Identifier::keyIsDisplay:
	interp.convertBooleanC(obj, ident, loc, nic_->isDisplay);
	return;
      case Identifier::keyScale:
	{
	  double d;
	  if (obj->realValue(d)) {
	    nic_->scaleType = FOTBuilder::symbolFalse;
	    nic_->scale[0] = nic_->scale[1] = d;
	  }
	  else if (obj->asSymbol()) {
	    static FOTBuilder::Symbol vals[] = {
	      FOTBuilder::symbolMax,
	      FOTBuilder::symbolMaxUniform
	    };
	    interp.convertEnumC(vals, 2, obj, ident, loc, nic_->scaleType);
	  }
	  else {
	    PairObj *pair = obj->asPair();
	    if (pair
	        && pair->car()->realValue(nic_->scale[0])
	        && (pair = pair->cdr()->asPair()) != 0
		&& pair->car()->realValue(nic_->scale[1])
		&& pair->cdr()->isNil()) {
	      nic_->scaleType = FOTBuilder::symbolFalse;
	    }
	    else
	      interp.invalidCharacteristicValue(ident, loc);
	  }
	}
	return;
      case Identifier::keyMaxWidth:
	if (interp.convertLengthSpecC(obj, ident, loc, nic_->maxWidth))
	  nic_->hasMaxWidth = 1;
	return;
      case Identifier::keyMaxHeight:
	if (interp.convertLengthSpecC(obj, ident, loc, nic_->maxHeight))
	  nic_->hasMaxHeight = 1;
	return;
      case Identifier::keyEntitySystemId:
	interp.convertStringC(obj, ident, loc, nic_->entitySystemId);
	return;
      case Identifier::keyNotationSystemId:
	interp.convertStringC(obj, ident, loc, nic_->notationSystemId);
	return;
      case Identifier::keyPositionPointX:
	interp.convertLengthSpecC(obj, ident, loc, nic_->positionPointX);
	return;
      case Identifier::keyPositionPointY:
	interp.convertLengthSpecC(obj, ident, loc, nic_->positionPointY);
	return;
      case Identifier::keyEscapementDirection:
	{
	  static FOTBuilder::Symbol vals[] = {
	    FOTBuilder::symbolTopToBottom,
	    FOTBuilder::symbolLeftToRight,
	    FOTBuilder::symbolBottomToTop,
	    FOTBuilder::symbolRightToLeft
	  };
	  interp.convertEnumC(vals, SIZEOF(vals), obj, ident, loc, nic_->escapementDirection);
	}
	return;
      case Identifier::keyBreakBeforePriority:
	interp.convertIntegerC(obj, ident, loc, nic_->breakBeforePriority);
	return;
      case Identifier::keyBreakAfterPriority:
	interp.convertIntegerC(obj, ident, loc, nic_->breakAfterPriority);
	return;
      default:
	break;
      }
    }
    CANNOT_HAPPEN();
  }
}

bool ExternalGraphicFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyIsDisplay:
    case Identifier::keyScale:
    case Identifier::keyMaxWidth:
    case Identifier::keyMaxHeight:
    case Identifier::keyEntitySystemId:
    case Identifier::keyNotationSystemId:
    case Identifier::keyPositionPointX:
    case Identifier::keyPositionPointY:
    case Identifier::keyEscapementDirection:
    case Identifier::keyBreakBeforePriority:
    case Identifier::keyBreakAfterPriority:
      return 1;
    default:
      break;
    }
  }
  return isDisplayNIC(ident);
}

FlowObj *ExternalGraphicFlowObj::copy(Collector &c) const
{
  return new (c) ExternalGraphicFlowObj(*this);
}

class RuleFlowObj : public FlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  RuleFlowObj();
  RuleFlowObj(const RuleFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
  bool ruleStyle(ProcessContext &, StyleObj *&style) {
    style = style_;
    return 1;
  }
  bool isRule() { return 1; }
private:
  Owner<FOTBuilder::RuleNIC> nic_;
};

RuleFlowObj::RuleFlowObj()
: nic_(new FOTBuilder::RuleNIC)
{
}

RuleFlowObj::RuleFlowObj(const RuleFlowObj &fo)
: FlowObj(fo), nic_(new FOTBuilder::RuleNIC(*fo.nic_))
{
}

void RuleFlowObj::processInner(ProcessContext &context)
{
  context.currentFOTBuilder().rule(*nic_);
}

void RuleFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					      const Location &loc, Interpreter &interp)
{
  if (setDisplayNIC(*nic_, ident, obj, loc, interp))
    return;
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyOrientation:
      {
	static FOTBuilder::Symbol vals[] = {
	  FOTBuilder::symbolHorizontal,
	  FOTBuilder::symbolVertical,
	  FOTBuilder::symbolEscapement,
	  FOTBuilder::symbolLineProgression
	};
	interp.convertEnumC(vals, SIZEOF(vals), obj, ident, loc, nic_->orientation);
      }
      return;
    case Identifier::keyLength:
      if (interp.convertLengthSpecC(obj, ident, loc, nic_->length))
	nic_->hasLength = 1;
      return;
    case Identifier::keyBreakBeforePriority:
      interp.convertIntegerC(obj, ident, loc, nic_->breakBeforePriority);
      return;
    case Identifier::keyBreakAfterPriority:
      interp.convertIntegerC(obj, ident, loc, nic_->breakAfterPriority);
      return;
    default:
      break;
    }
  }
  CANNOT_HAPPEN();
}

bool RuleFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyOrientation:
    case Identifier::keyLength:
    case Identifier::keyBreakBeforePriority:
    case Identifier::keyBreakAfterPriority:
      return 1;
    default:
      break;
    }
  }
  return isDisplayNIC(ident);
}

FlowObj *RuleFlowObj::copy(Collector &c) const
{
  return new (c) RuleFlowObj(*this);
}

class AlignmentPointFlowObj : public FlowObj {
public:
  AlignmentPointFlowObj() { }
  FlowObj *copy(Collector &) const;
  void processInner(ProcessContext &);
};

FlowObj *AlignmentPointFlowObj::copy(Collector &c) const
{
  return new (c) AlignmentPointFlowObj(*this);
}

void AlignmentPointFlowObj::processInner(ProcessContext &context)
{
  context.currentFOTBuilder().alignmentPoint();
}

class SidelineFlowObj : public CompoundFlowObj {
public:
  SidelineFlowObj() { }
  FlowObj *copy(Collector &) const;
  void processInner(ProcessContext &);
};

FlowObj *SidelineFlowObj::copy(Collector &c) const
{
  return new (c) SidelineFlowObj(*this);
}

void SidelineFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startSideline();
  CompoundFlowObj::processInner(context);
  fotb.endSideline();
}
void SequenceFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startSequence();
  CompoundFlowObj::processInner(context);
  fotb.endSequence();
}

FlowObj *SequenceFlowObj::copy(Collector &c) const
{
  return new (c) SequenceFlowObj(*this);
}

class LineFieldFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  LineFieldFlowObj() : nic_(new FOTBuilder::LineFieldNIC) { }
  LineFieldFlowObj(const LineFieldFlowObj &fo)
    : CompoundFlowObj(fo), nic_(new FOTBuilder::LineFieldNIC(*fo.nic_)) { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  bool hasNonInheritedC(const Identifier *ident) const;
  void setNonInheritedC(const Identifier *ident, ELObj *obj,
			const Location &loc, Interpreter &interp);
private:
  Owner<FOTBuilder::LineFieldNIC> nic_;
};

FlowObj *LineFieldFlowObj::copy(Collector &c) const
{
  return new (c) LineFieldFlowObj(*this);
}

void LineFieldFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startLineField(*nic_);
  CompoundFlowObj::processInner(context);
  fotb.endLineField();
}

void LineFieldFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					const Location &loc, Interpreter &interp)
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyBreakBeforePriority:
      interp.convertIntegerC(obj, ident, loc, nic_->breakBeforePriority);
      return;
    case Identifier::keyBreakAfterPriority:
      interp.convertIntegerC(obj, ident, loc, nic_->breakAfterPriority);
      return;
    default:
      break;
    }
  }
  CANNOT_HAPPEN();
}

bool LineFieldFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyBreakBeforePriority:
    case Identifier::keyBreakAfterPriority:
      return 1;
    default:
      break;
    }
  }
  return 0;
}

class SimplePageSequenceFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  struct HeaderFooter {
    enum { nParts = 6 };
    HeaderFooter();
    SosofoObj *part[nParts];
  };
  SimplePageSequenceFlowObj();
  SimplePageSequenceFlowObj(const SimplePageSequenceFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  bool hasNonInheritedC(const Identifier *) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  void traceSubObjects(Collector &) const;
private:
  enum { nPageTypeBits = 2 };
  Owner<HeaderFooter> hf_;
};


SimplePageSequenceFlowObj::SimplePageSequenceFlowObj()
: hf_(new HeaderFooter)
{
  hasSubObjects_ = 1;
}

SimplePageSequenceFlowObj::SimplePageSequenceFlowObj(const SimplePageSequenceFlowObj &fo)
: CompoundFlowObj(fo), hf_(new HeaderFooter(*fo.hf_))
{
}

void SimplePageSequenceFlowObj::traceSubObjects(Collector &c) const
{
  for (int i = 0; i < HeaderFooter::nParts; i++)
    c.trace(hf_->part[i]);
  CompoundFlowObj::traceSubObjects(c);
}

void SimplePageSequenceFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startSimplePageSequence();
  for (int i = 0; i < (1 << nPageTypeBits); i++) {
    context.setPageType(i);
    for (int j = 0; j < HeaderFooter::nParts; j++) {
      fotb.startSimplePageSequenceHeaderFooter(i | (j << nPageTypeBits));
      if (hf_->part[j])
	hf_->part[j]->process(context);
      fotb.endSimplePageSequenceHeaderFooter(i | (j << nPageTypeBits));
    }
  }
  fotb.endAllSimplePageSequenceHeaderFooter();
  context.clearPageType();
  CompoundFlowObj::processInner(context);
  fotb.endSimplePageSequence();
}

void SimplePageSequenceFlowObj::setNonInheritedC(const Identifier *ident,
						 ELObj *obj,
						 const Location &loc,
						 Interpreter &interp)
{
  SosofoObj *sosofo = obj->asSosofo();
  if (!sosofo) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::invalidCharacteristicValue,
		   StringMessageArg(ident->name()));
    return;
  }
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyLeftHeader:
      hf_->part[(FOTBuilder::leftHF | FOTBuilder::headerHF) >> nPageTypeBits] = sosofo;
      return;
    case Identifier::keyCenterHeader:
      hf_->part[(FOTBuilder::centerHF | FOTBuilder::headerHF) >> nPageTypeBits] = sosofo;
      return;
    case Identifier::keyRightHeader:
      hf_->part[(FOTBuilder::rightHF | FOTBuilder::headerHF) >> nPageTypeBits] = sosofo;
      return;
    case Identifier::keyLeftFooter:
      hf_->part[(FOTBuilder::leftHF | FOTBuilder::footerHF) >> nPageTypeBits] = sosofo;
      return;
    case Identifier::keyCenterFooter:
      hf_->part[(FOTBuilder::centerHF | FOTBuilder::footerHF) >> nPageTypeBits] = sosofo;
      return;
    case Identifier::keyRightFooter:
      hf_->part[(FOTBuilder::rightHF | FOTBuilder::footerHF) >> nPageTypeBits] = sosofo;
      return;
    default:
      break;
    }
  }
  CANNOT_HAPPEN();
}

bool SimplePageSequenceFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyLeftHeader:
    case Identifier::keyCenterHeader:
    case Identifier::keyRightHeader:
    case Identifier::keyLeftFooter:
    case Identifier::keyCenterFooter:
    case Identifier::keyRightFooter:
      return 1;
    default:
      break;
    }
  }
  return 0;
}

FlowObj *SimplePageSequenceFlowObj::copy(Collector &c) const
{
  return new (c) SimplePageSequenceFlowObj(*this);
}

SimplePageSequenceFlowObj::HeaderFooter::HeaderFooter()
{
  for (int i = 0; i < nParts; i++)
    part[i] = 0;
}

class LinkFlowObj : public CompoundFlowObj {
public:
  LinkFlowObj();
  FlowObj *copy(Collector &) const;
  void processInner(ProcessContext &);
  void traceSubObjects(Collector &) const;
  bool hasNonInheritedC(const Identifier *) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
private:
  AddressObj *addressObj_;
};

LinkFlowObj::LinkFlowObj()
: addressObj_(0)
{
}

void LinkFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  if (!addressObj_) {
    FOTBuilder::Address addr;
    addr.type = FOTBuilder::Address::none;
    fotb.startLink(addr);
  }
  else
    fotb.startLink(addressObj_->address());
  CompoundFlowObj::processInner(context);
  fotb.endLink();
}

FlowObj *LinkFlowObj::copy(Collector &c) const
{
  return new (c) LinkFlowObj(*this);
}

void LinkFlowObj::traceSubObjects(Collector &c) const
{
  CompoundFlowObj::traceSubObjects(c);
  c.trace(addressObj_);
}

bool LinkFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  return ident->syntacticKey(key) && key == Identifier::keyDestination;
}

void LinkFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
				   const Location &loc, Interpreter &interp)
{
  AddressObj *address = obj->asAddress();
  if (!address) {
    if (obj != interp.makeFalse()) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidCharacteristicValue,
                     StringMessageArg(ident->name()));
    }
    else
      address = interp.makeAddressNone();
  }
  addressObj_ = address;
}

class ScrollFlowObj : public CompoundFlowObj {
public:
  ScrollFlowObj() { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
};

void ScrollFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startScroll();
  CompoundFlowObj::processInner(context);
  fotb.endScroll();
}

FlowObj *ScrollFlowObj::copy(Collector &c) const
{
  return new (c) ScrollFlowObj(*this);
}

class MarginaliaFlowObj : public CompoundFlowObj {
public:
  MarginaliaFlowObj() { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
};

void MarginaliaFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startMarginalia();
  CompoundFlowObj::processInner(context);
  fotb.endMarginalia();
}

FlowObj *MarginaliaFlowObj::copy(Collector &c) const
{
  return new (c) MarginaliaFlowObj(*this);
}

class MultiModeFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  MultiModeFlowObj();
  MultiModeFlowObj(const MultiModeFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  bool hasNonInheritedC(const Identifier *) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  struct NIC {
    NIC() : hasPrincipalMode(0) { }
    bool hasPrincipalMode;
    FOTBuilder::MultiMode principalMode;
    Vector<FOTBuilder::MultiMode> namedModes;
  };
private:
  bool handleMultiModesMember(const Identifier *, ELObj *obj,
			      const Location &, Interpreter &);
  Owner<NIC> nic_;
};

MultiModeFlowObj::MultiModeFlowObj()
: nic_(new NIC)
{
}

MultiModeFlowObj::MultiModeFlowObj(const MultiModeFlowObj &fo)
: CompoundFlowObj(fo), nic_(new NIC(*fo.nic_))
{
}

void MultiModeFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  Vector<FOTBuilder *> fotbs(nic_->namedModes.size());
  fotb.startMultiMode(nic_->hasPrincipalMode ? &nic_->principalMode : 0,
		      nic_->namedModes,
		      fotbs);
  Vector<SymbolObj *> portSyms(nic_->namedModes.size());
  for (size_t i = 0; i < portSyms.size(); i++)
    portSyms[i] = context.vm().interp->makeSymbol(nic_->namedModes[i].name);
  context.pushPorts(nic_->hasPrincipalMode, portSyms, fotbs);
  CompoundFlowObj::processInner(context);
  context.popPorts();
  fotb.endMultiMode();
}

FlowObj *MultiModeFlowObj::copy(Collector &c) const
{
  return new (c) MultiModeFlowObj(*this);
}

bool MultiModeFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  return ident->syntacticKey(key) && key == Identifier::keyMultiModes;
}

void MultiModeFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					const Location &loc, Interpreter &interp)
{
  while (!obj->isNil()) {
    PairObj *pair = obj->asPair();
    if (!pair || !handleMultiModesMember(ident, pair->car(), loc, interp)) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidCharacteristicValue,
  		     StringMessageArg(ident->name()));
      return;
    }
    obj = pair->cdr();
  }
}

bool MultiModeFlowObj::handleMultiModesMember(const Identifier *, ELObj *obj,
					      const Location &loc, Interpreter &interp)
{
  if (obj == interp.makeFalse()) {
    nic_->hasPrincipalMode = 1;
    return 1;
  }
  SymbolObj *sym = obj->asSymbol();
  if (sym) {
    nic_->namedModes.resize(nic_->namedModes.size() + 1);
    nic_->namedModes.back().name = *sym->name();
    return 1;
  }
  PairObj *pair = obj->asPair();
  if (!pair)
    return 0;
  ELObj *spec = pair->car();
  pair = pair->cdr()->asPair();
  if (!pair || !pair->cdr()->isNil())
    return 0;
  const Char *s;
  size_t n;
  if (!pair->car()->stringData(s, n))
    return 0;
  if (spec == interp.makeFalse()) {
    nic_->hasPrincipalMode = 1;
    nic_->principalMode.hasDesc = 1;
    nic_->principalMode.desc.assign(s, n);
    return 1;
  }
  sym = spec->asSymbol();
  if (!sym)
    return 0;
  nic_->namedModes.resize(nic_->namedModes.size() + 1);
  nic_->namedModes.back().name = *sym->name();
  nic_->namedModes.back().desc.assign(s, n);
  nic_->namedModes.back().hasDesc = 1;
  return 1;
}


class ScoreFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  ScoreFlowObj();
  FlowObj *copy(Collector &) const;
  void processInner(ProcessContext &);
  bool hasNonInheritedC(const Identifier *) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  class Type {
  public:
    virtual ~Type();
    virtual void start(FOTBuilder &) = 0;
    virtual Type *copy() const = 0;
  };
private:
  class SymbolType : public Type {
  public:
    SymbolType(FOTBuilder::Symbol type) : type_(type) { }
    void start(FOTBuilder &);
    Type *copy() const;
  private:
    FOTBuilder::Symbol type_;
  };
  class LengthSpecType : public Type {
  public:
    LengthSpecType(long n) : len_(n) { }
    void start(FOTBuilder &);
    Type *copy() const;
  private:
    FOTBuilder::LengthSpec len_;
  };
  class CharType : public Type {
  public:
    CharType(Char c) : c_(c) { }
    void start(FOTBuilder &);
    Type *copy() const;
  private:
    Char c_;
  };
  CopyOwner<Type> type_;
};

ScoreFlowObj::ScoreFlowObj()
{
}

FlowObj *ScoreFlowObj::copy(Collector &c) const
{
  return new (c) ScoreFlowObj(*this);
}

void ScoreFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  if (type_)
    type_->start(fotb);
  else
    fotb.startSequence();
  CompoundFlowObj::processInner(context);
  if (type_)
    fotb.endScore();
  else
    fotb.endSequence();
}

bool ScoreFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  return ident->syntacticKey(key) && key == Identifier::keyType;
}

void ScoreFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
				    const Location &loc, Interpreter &interp)
{
  Char c;
  if (obj->charValue(c)) {
    type_ = new CharType(c);
    return;
  }
  double d;
  long n;
  int dim;
  switch (obj->quantityValue(n, d, dim)) {
  case longQuantity:
    if (dim == 1) {
      type_ = new LengthSpecType(n);
      return;
    }
    break;
  case doubleQuantity:
    if (dim == 1) {
      type_ = new LengthSpecType(long(d));
      return;
    }
    break;
  default:
    break;
  }
  static FOTBuilder::Symbol vals[] = {
    FOTBuilder::symbolBefore,
    FOTBuilder::symbolThrough,
    FOTBuilder::symbolAfter
  };
  FOTBuilder::Symbol sym;
  if (interp.convertEnumC(vals, SIZEOF(vals), obj, ident, loc, sym))
    type_ = new SymbolType(sym);
}

ScoreFlowObj::Type::~Type()
{
}

void ScoreFlowObj::SymbolType::start(FOTBuilder &fotb)
{
  fotb.startScore(type_);
}

void ScoreFlowObj::CharType::start(FOTBuilder &fotb)
{
  fotb.startScore(c_);
}

void ScoreFlowObj::LengthSpecType::start(FOTBuilder &fotb)
{
  fotb.startScore(len_);
}

ScoreFlowObj::Type *ScoreFlowObj::SymbolType::copy() const
{
  return new SymbolType(*this);
}

ScoreFlowObj::Type *ScoreFlowObj::LengthSpecType::copy() const
{
  return new LengthSpecType(*this);
}

ScoreFlowObj::Type *ScoreFlowObj::CharType::copy() const
{
  return new CharType(*this);
}

class BoxFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  BoxFlowObj();
  BoxFlowObj(const BoxFlowObj &);
  FlowObj *copy(Collector &) const;
  void processInner(ProcessContext &);
  bool hasNonInheritedC(const Identifier *) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
private:
  Owner<FOTBuilder::BoxNIC> nic_;
};

BoxFlowObj::BoxFlowObj()
: nic_(new FOTBuilder::BoxNIC)
{
}

BoxFlowObj::BoxFlowObj(const BoxFlowObj &fo)
: CompoundFlowObj(fo), nic_(new FOTBuilder::BoxNIC(*fo.nic_))
{
}

void BoxFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startBox(*nic_);
  CompoundFlowObj::processInner(context);
  fotb.endBox();
}

void BoxFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					      const Location &loc, Interpreter &interp)
{
  if (setDisplayNIC(*nic_, ident, obj, loc, interp))
    return;
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyIsDisplay:
      interp.convertBooleanC(obj, ident, loc, nic_->isDisplay);
      return;
    case Identifier::keyBreakBeforePriority:
      interp.convertIntegerC(obj, ident, loc, nic_->breakBeforePriority);
      return;
    case Identifier::keyBreakAfterPriority:
      interp.convertIntegerC(obj, ident, loc, nic_->breakAfterPriority);
      return;
    default:
      break;
    }
  }
  CANNOT_HAPPEN();
}

bool BoxFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyIsDisplay:
    case Identifier::keyBreakBeforePriority:
    case Identifier::keyBreakAfterPriority:
      return 1;
    default:
      break;
    }
  }
  return isDisplayNIC(ident);
}

FlowObj *BoxFlowObj::copy(Collector &c) const
{
  return new (c) BoxFlowObj(*this);
}

class LeaderFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  LeaderFlowObj();
  LeaderFlowObj(const LeaderFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
private:
  Owner<FOTBuilder::LeaderNIC> nic_;
};

LeaderFlowObj::LeaderFlowObj()
: nic_(new FOTBuilder::LeaderNIC)
{
}

LeaderFlowObj::LeaderFlowObj(const LeaderFlowObj &fo)
: CompoundFlowObj(fo), nic_(new FOTBuilder::LeaderNIC(*fo.nic_))
{
}

void LeaderFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startLeader(*nic_);
  CompoundFlowObj::processInner(context);
  fotb.endLeader();
}

void LeaderFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
				     const Location &loc, Interpreter &interp)
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyLength:
      if (interp.convertLengthSpecC(obj, ident, loc, nic_->length))
	nic_->hasLength = 1;
      return;
    case Identifier::keyBreakBeforePriority:
      interp.convertIntegerC(obj, ident, loc, nic_->breakBeforePriority);
      return;
    case Identifier::keyBreakAfterPriority:
      interp.convertIntegerC(obj, ident, loc, nic_->breakAfterPriority);
      return;
    default:
      break;
    }
  }
  CANNOT_HAPPEN();
}

bool LeaderFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyLength:
    case Identifier::keyBreakBeforePriority:
    case Identifier::keyBreakAfterPriority:
      return 1;
    default:
      break;
    }
  }
  return 0;
}

FlowObj *LeaderFlowObj::copy(Collector &c) const
{
  return new (c) LeaderFlowObj(*this);
}

class CharacterFlowObj : public FlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  CharacterFlowObj();
  CharacterFlowObj(const CharacterFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
  bool characterStyle(ProcessContext &, StyleObj *&style, FOTBuilder::CharacterNIC &nic) {
    style = style_;
    nic = *nic_;
    return 1;
  }
  bool isCharacter() { return 1; }
private:
  Owner<FOTBuilder::CharacterNIC> nic_;
};

CharacterFlowObj::CharacterFlowObj()
: nic_(new FOTBuilder::CharacterNIC)
{
}

CharacterFlowObj::CharacterFlowObj(const CharacterFlowObj &fo)
: FlowObj(fo), nic_(new FOTBuilder::CharacterNIC(*fo.nic_))
{
}

void CharacterFlowObj::processInner(ProcessContext &context)
{
  context.currentFOTBuilder().character(*nic_);
}

void CharacterFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					const Location &loc, Interpreter &interp)
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyIsSpace:
      if (interp.convertBooleanC(obj, ident, loc, nic_->isSpace))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cIsSpace);
      return;
    case Identifier::keyIsRecordEnd:
      if (interp.convertBooleanC(obj, ident, loc, nic_->isRecordEnd))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cIsRecordEnd);
      return;
    case Identifier::keyIsInputTab:
      if (interp.convertBooleanC(obj, ident, loc, nic_->isInputTab))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cIsInputTab);
      return;
    case Identifier::keyIsInputWhitespace:
      if (interp.convertBooleanC(obj, ident, loc, nic_->isInputWhitespace))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cIsInputWhitespace);
      return;
    case Identifier::keyIsPunct:
      if (interp.convertBooleanC(obj, ident, loc, nic_->isPunct))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cIsPunct);
      return;
    case Identifier::keyIsDropAfterLineBreak:
      if (interp.convertBooleanC(obj, ident, loc, nic_->isDropAfterLineBreak))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cIsDropAfterLineBreak);
      return;
    case Identifier::keyIsDropUnlessBeforeLineBreak:
      if (interp.convertBooleanC(obj, ident, loc, nic_->isDropUnlessBeforeLineBreak))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cIsDropUnlessBeforeLineBreak);
      return;
    case Identifier::keyBreakBeforePriority:
      if (interp.convertIntegerC(obj, ident, loc, nic_->breakBeforePriority))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cBreakBeforePriority);
      return;
    case Identifier::keyBreakAfterPriority:
      if (interp.convertIntegerC(obj, ident, loc, nic_->breakAfterPriority))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cBreakAfterPriority);
      return;
    case Identifier::keyScript:
      {
	if (obj == interp.makeFalse())
	  nic_->script = 0;
	else {
	  StringC tem;
	  if (!interp.convertStringC(obj, ident, loc, tem))
	    return;
	  nic_->script = interp.storePublicId(tem.data(), tem.size(), loc);
	}
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cScript);
      }
      return;
    case Identifier::keyChar:
      if (interp.convertCharC(obj, ident, loc, nic_->ch))
        nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cChar);
      return;
    case Identifier::keyGlyphId:
      {
	if (obj == interp.makeFalse()) {
	  nic_->glyphId = FOTBuilder::GlyphId();
	  nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cGlyphId);
	  return;
	}
	const FOTBuilder::GlyphId *p = obj->glyphId();
	if (!p) {
	  interp.setNextLocation(loc);
	  interp.message(InterpreterMessages::invalidCharacteristicValue,
  		         StringMessageArg(ident->name()));
	  return;
	}
	nic_->glyphId = *p;
	nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cGlyphId);
      }
      return;
    case Identifier::keyMathFontPosture:
      {
	static FOTBuilder::Symbol vals[] = {
	  FOTBuilder::symbolFalse,
	  FOTBuilder::symbolNotApplicable,
	  FOTBuilder::symbolUpright,
	  FOTBuilder::symbolOblique,
	  FOTBuilder::symbolBackSlantedOblique,
	  FOTBuilder::symbolItalic,
	  FOTBuilder::symbolBackSlantedItalic,
	};
	if (interp.convertEnumC(vals, SIZEOF(vals), obj, ident, loc, nic_->mathFontPosture))
	  nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cMathFontPosture);
      }
      return;
    case Identifier::keyMathClass:
      {
	static FOTBuilder::Symbol vals[] = {
	  FOTBuilder::symbolOrdinary,
	  FOTBuilder::symbolOperator,
	  FOTBuilder::symbolBinary,
	  FOTBuilder::symbolRelation,
	  FOTBuilder::symbolOpening,
	  FOTBuilder::symbolClosing,
	  FOTBuilder::symbolPunctuation,
	  FOTBuilder::symbolInner,
	  FOTBuilder::symbolSpace,
	};
	if (interp.convertEnumC(vals, SIZEOF(vals), obj, ident, loc, nic_->mathClass))
	  nic_->specifiedC |= (1 << FOTBuilder::CharacterNIC::cMathClass);
      }
      return;
    case Identifier::keyStretchFactor:
      interp.convertRealC(obj, ident, loc, nic_->stretchFactor);
      return;
    default:
      break;
    }
  }
  CANNOT_HAPPEN();
}

bool CharacterFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyChar:
    case Identifier::keyGlyphId:
    case Identifier::keyIsSpace:
    case Identifier::keyIsRecordEnd:
    case Identifier::keyIsInputTab:
    case Identifier::keyIsInputWhitespace:
    case Identifier::keyIsPunct:
    case Identifier::keyIsDropAfterLineBreak:
    case Identifier::keyIsDropUnlessBeforeLineBreak:
    case Identifier::keyScript:
    case Identifier::keyMathClass:
    case Identifier::keyMathFontPosture:
    case Identifier::keyStretchFactor:
    case Identifier::keyBreakBeforePriority:
    case Identifier::keyBreakAfterPriority:
      return 1;
    default:
      break;
    }
  }
  return 0;
}

FlowObj *CharacterFlowObj::copy(Collector &c) const
{
  return new (c) CharacterFlowObj(*this);
}

class MathSequenceFlowObj : public CompoundFlowObj {
public:
  MathSequenceFlowObj() { }
  void processInner(ProcessContext &context) {
    FOTBuilder &fotb = context.currentFOTBuilder();
    fotb.startMathSequence();
    CompoundFlowObj::processInner(context);
    fotb.endMathSequence();
  }
  FlowObj *copy(Collector &c) const {
    return new (c) MathSequenceFlowObj(*this);
  }
};

class FractionFlowObj : public CompoundFlowObj {
public:
  FractionFlowObj() { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
};

void FractionFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  Vector<FOTBuilder *> fotbs(2);
  fotb.startFraction(fotbs[0], fotbs[1]);
  Vector<size_t> dep;
  StyleObj *fractionBarStyle = 0;
  SosofoObj *sosofo
    = context.currentStyleStack().actual(context.vm().interp->fractionBarC(),
					 *context.vm().interp, dep)->asSosofo();
  if (sosofo)
    sosofo->ruleStyle(context, fractionBarStyle);
  if (fractionBarStyle)
    context.currentStyleStack().push(fractionBarStyle, context.vm(), fotb);
  fotb.fractionBar();
  if (fractionBarStyle)
    context.currentStyleStack().pop();
  Vector<SymbolObj *> labels(2);
  labels[0] = context.vm().interp->portName(Interpreter::portNumerator);
  labels[1] = context.vm().interp->portName(Interpreter::portDenominator);
  context.pushPorts(0, labels, fotbs);
  // Fraction flow object doesn't have principal port,
  // so clear the current connection.
  CompoundFlowObj::processInner(context);
  context.popPorts();
  fotb.endFraction();
}

FlowObj *FractionFlowObj::copy(Collector &c) const
{
  return new (c) FractionFlowObj(*this);
}

class UnmathFlowObj : public CompoundFlowObj {
public:
  UnmathFlowObj() { }
  void processInner(ProcessContext &context) {
    FOTBuilder &fotb = context.currentFOTBuilder();
    fotb.startUnmath();
    CompoundFlowObj::processInner(context);
    fotb.endUnmath();
  }
  FlowObj *copy(Collector &c) const {
    return new (c) UnmathFlowObj(*this);
  }
};

class SuperscriptFlowObj : public CompoundFlowObj {
public:
  SuperscriptFlowObj() { }
  void processInner(ProcessContext &context) {
    FOTBuilder &fotb = context.currentFOTBuilder();
    fotb.startSuperscript();
    CompoundFlowObj::processInner(context);
    fotb.endSuperscript();
  }
  FlowObj *copy(Collector &c) const {
    return new (c) SuperscriptFlowObj(*this);
  }
};

class SubscriptFlowObj : public CompoundFlowObj {
public:
  SubscriptFlowObj() { }
  void processInner(ProcessContext &context) {
    FOTBuilder &fotb = context.currentFOTBuilder();
    fotb.startSubscript();
    CompoundFlowObj::processInner(context);
    fotb.endSubscript();
  }
  FlowObj *copy(Collector &c) const {
    return new (c) SubscriptFlowObj(*this);
  }
};

class ScriptFlowObj : public CompoundFlowObj {
public:
  ScriptFlowObj() { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &c) const {
    return new (c) ScriptFlowObj(*this);
  }

};

void ScriptFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  Vector<FOTBuilder *> fotbs(6);
  fotb.startScript(fotbs[0], fotbs[1], fotbs[2], fotbs[3],
                   fotbs[4], fotbs[5]);
  Vector<SymbolObj *> labels(6);
  labels[0] = context.vm().interp->portName(Interpreter::portPreSup);
  labels[1] = context.vm().interp->portName(Interpreter::portPreSub);
  labels[2] = context.vm().interp->portName(Interpreter::portPostSup);
  labels[3] = context.vm().interp->portName(Interpreter::portPostSub);
  labels[4] = context.vm().interp->portName(Interpreter::portMidSup);
  labels[5] = context.vm().interp->portName(Interpreter::portMidSub);
  context.pushPorts(1, labels, fotbs);
  CompoundFlowObj::processInner(context);
  context.popPorts();
  fotb.endScript();
}

class MarkFlowObj : public CompoundFlowObj {
public:
  MarkFlowObj() { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &c) const {
    return new (c) MarkFlowObj(*this);
  }
};

void MarkFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  Vector<FOTBuilder *> fotbs(2);
  fotb.startMark(fotbs[0], fotbs[1]);
  Vector<SymbolObj *> labels(2);
  labels[0] = context.vm().interp->portName(Interpreter::portOverMark);
  labels[1] = context.vm().interp->portName(Interpreter::portUnderMark);
  context.pushPorts(1, labels, fotbs);
  CompoundFlowObj::processInner(context);
  context.popPorts();
  fotb.endMark();
}

class FenceFlowObj : public CompoundFlowObj {
public:
  FenceFlowObj() { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &c) const {
    return new (c) FenceFlowObj(*this);
  }
};

void FenceFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  Vector<FOTBuilder *> fotbs(2);
  fotb.startFence(fotbs[0], fotbs[1]);
  Vector<SymbolObj *> labels(2);
  labels[0] = context.vm().interp->portName(Interpreter::portOpen);
  labels[1] = context.vm().interp->portName(Interpreter::portClose);
  context.pushPorts(1, labels, fotbs);
  CompoundFlowObj::processInner(context);
  context.popPorts();
  fotb.endFence();
}

class RadicalFlowObj : public CompoundFlowObj {
public:
  RadicalFlowObj() : radical_(0) { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &c) const {
    return new (c) RadicalFlowObj(*this);
  }
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
  void traceSubObjects(Collector &c) const {
    c.trace(radical_);
    CompoundFlowObj::traceSubObjects(c);
  }
private:
  SosofoObj *radical_;
};

void RadicalFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  Vector<FOTBuilder *> fotbs(1);
  fotb.startRadical(fotbs[0]);
  StyleObj *style;
  FOTBuilder::CharacterNIC nic;
  if (radical_ && radical_->characterStyle(context, style, nic)) {
    if (style)
      context.currentStyleStack().push(style, context.vm(), fotb);
    fotb.radicalRadical(nic);
    if (style)
      context.currentStyleStack().pop();
  }
  else 
    fotb.radicalRadicalDefaulted();
  Vector<SymbolObj *> labels(1);
  labels[0] = context.vm().interp->portName(Interpreter::portDegree);
  context.pushPorts(1, labels, fotbs);
  CompoundFlowObj::processInner(context);
  context.popPorts();
  fotb.endRadical();
}

bool RadicalFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  return ident->syntacticKey(key) && key == Identifier::keyRadical;
}

void RadicalFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
				      const Location &loc, Interpreter &interp)
{
  radical_ = obj->asSosofo();
  if (!radical_ || !radical_->isCharacter()) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::invalidCharacteristicValue,
		   StringMessageArg(ident->name()));
  }
}

class MathOperatorFlowObj : public CompoundFlowObj {
public:
  MathOperatorFlowObj() { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &c) const {
    return new (c) MathOperatorFlowObj(*this);
  }
};

void MathOperatorFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  Vector<FOTBuilder *> fotbs(3);
  fotb.startMathOperator(fotbs[0], fotbs[1], fotbs[2]);
  Vector<SymbolObj *> labels(3);
  labels[0] = context.vm().interp->portName(Interpreter::portOperator);
  labels[1] = context.vm().interp->portName(Interpreter::portLowerLimit);
  labels[2] = context.vm().interp->portName(Interpreter::portUpperLimit);
  context.pushPorts(1, labels, fotbs);
  CompoundFlowObj::processInner(context);
  context.popPorts();
  fotb.endMathOperator();
}

class GridFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  GridFlowObj();
  GridFlowObj(const GridFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &c) const {
    return new (c) GridFlowObj(*this);
  }
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
private:
  Owner<FOTBuilder::GridNIC> nic_;
};

GridFlowObj::GridFlowObj()
: nic_(new FOTBuilder::GridNIC)
{
}

GridFlowObj::GridFlowObj(const GridFlowObj &fo)
: CompoundFlowObj(fo), nic_(new FOTBuilder::GridNIC(*fo.nic_))
{
}

void GridFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startGrid(*nic_);
  CompoundFlowObj::processInner(context);
  fotb.endGrid();
}

void GridFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
					const Location &loc, Interpreter &interp)
{
  long tem;
  if (!interp.convertIntegerC(obj, ident, loc, tem))
    return;
  if (tem <= 0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::invalidCharacteristicValue,
		   StringMessageArg(ident->name()));
    return;
  }
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyGridNColumns:
      nic_->nColumns = tem;
      return;
    case Identifier::keyGridNRows:
      nic_->nRows = tem;
      return;
    default:
      break;
    }
  }
  CANNOT_HAPPEN();
}

bool GridFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyGridNColumns:
    case Identifier::keyGridNRows:
      return 1;
    default:
      break;
    }
  }
  return 0;
}

class GridCellFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  GridCellFlowObj();
  GridCellFlowObj(const GridCellFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &c) const {
    return new (c) GridCellFlowObj(*this);
  }
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
  bool hasNonInheritedC(const Identifier *) const;
private:
  Owner<FOTBuilder::GridCellNIC> nic_;
};

GridCellFlowObj::GridCellFlowObj()
: nic_(new FOTBuilder::GridCellNIC)
{
}

GridCellFlowObj::GridCellFlowObj(const GridCellFlowObj &fo)
: CompoundFlowObj(fo), nic_(new FOTBuilder::GridCellNIC(*fo.nic_))
{
}

void GridCellFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startGridCell(*nic_);
  CompoundFlowObj::processInner(context);
  fotb.endGridCell();
}

void GridCellFlowObj::setNonInheritedC(const Identifier *ident, ELObj *obj,
				       const Location &loc, Interpreter &interp)
{
  long tem;
  if (!interp.convertIntegerC(obj, ident, loc, tem))
    return;
  if (tem <= 0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::invalidCharacteristicValue,
		   StringMessageArg(ident->name()));
    return;
  }
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyColumnNumber:
      nic_->columnNumber = tem;
      return;
    case Identifier::keyRowNumber:
      nic_->rowNumber = tem;
      return;
    default:
      break;
    }
  }
  CANNOT_HAPPEN();
}

bool GridCellFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  Identifier::SyntacticKey key;
  if (ident->syntacticKey(key)) {
    switch (key) {
    case Identifier::keyColumnNumber:
    case Identifier::keyRowNumber:
      return 1;
    default:
      break;
    }
  }
  return 0;
}

class TableFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  struct NIC : public FOTBuilder::TableNIC {
    NIC() : beforeRowBorder(0), afterRowBorder(0),
            beforeColumnBorder(0), afterColumnBorder(0) { }
    StyleObj *beforeRowBorder;
    StyleObj *afterRowBorder;
    StyleObj *beforeColumnBorder;
    StyleObj *afterColumnBorder;
  };
  TableFlowObj() : nic_(new NIC) { }
  TableFlowObj(const TableFlowObj &fo) : CompoundFlowObj(fo), nic_(new NIC(*fo.nic_)) { }
  void processInner(ProcessContext &context) {
    context.startTable();
    FOTBuilder &fotb = context.currentFOTBuilder();
    fotb.startTable(*nic_);
    Interpreter &interp = *context.vm().interp;
    Vector<size_t> dep;
    ELObj *obj = context.currentStyleStack().actual(interp.tableBorderC(), interp, dep);
    StyleObj *borderStyle;
    if (obj == interp.makeFalse())
      borderStyle = interp.borderFalseStyle();
    else if (obj == interp.makeTrue())
      borderStyle = interp.borderTrueStyle();
    else {
      SosofoObj *sosofo = obj->asSosofo();
      if (!sosofo || !sosofo->tableBorderStyle(borderStyle))
	borderStyle = 0;
    }
    border(nic_->beforeRowBorder, borderStyle, &FOTBuilder::tableBeforeRowBorder, context);
    border(nic_->afterRowBorder, borderStyle, &FOTBuilder::tableAfterRowBorder, context);
    border(nic_->beforeColumnBorder, borderStyle, &FOTBuilder::tableBeforeColumnBorder, context);
    border(nic_->afterColumnBorder, borderStyle, &FOTBuilder::tableAfterColumnBorder, context);
    CompoundFlowObj::processInner(context);
    if (context.inTableRow())
      context.endTableRow();
    context.endTable();
    fotb.endTable();
  }
  FlowObj *copy(Collector &c) const {
    return new (c) TableFlowObj(*this);
  }
  bool hasNonInheritedC(const Identifier *ident) const {
    Identifier::SyntacticKey key;
    if (ident->syntacticKey(key)) {
      switch (key) {
      case Identifier::keyBeforeRowBorder:
      case Identifier::keyAfterRowBorder:
      case Identifier::keyBeforeColumnBorder:
      case Identifier::keyAfterColumnBorder:
      case Identifier::keyTableWidth:
	return 1;
      default:
	break;
      }
    }
    return isDisplayNIC(ident);
  }
  void setNonInheritedC(const Identifier *ident, ELObj *obj,
                        const Location &loc, Interpreter &interp) {
    if (setDisplayNIC(*nic_, ident, obj, loc, interp))
      return;
    Identifier::SyntacticKey key;
    if (!ident->syntacticKey(key))
      CANNOT_HAPPEN();
    if (key == Identifier::keyTableWidth) {
      if (obj == interp.makeFalse())
	nic_->widthType = FOTBuilder::TableNIC::widthMinimum;
      else if (interp.convertLengthSpecC(obj, ident, loc, nic_->width))
	nic_->widthType = FOTBuilder::TableNIC::widthExplicit;
      return;
    }
    StyleObj *style;
    SosofoObj *sosofo = obj->asSosofo();
    if (!sosofo || !sosofo->tableBorderStyle(style)) {
      Boolean b;
      if (!interp.convertBooleanC(obj, ident, loc, b))
	return;
      style = b ? interp.borderTrueStyle() : interp.borderFalseStyle();
    }
    switch (key) {
    case Identifier::keyBeforeRowBorder:
      nic_->beforeRowBorder = style;
      break;
    case Identifier::keyAfterRowBorder:
      nic_->afterRowBorder = style;
      break;
    case Identifier::keyBeforeColumnBorder:
      nic_->beforeColumnBorder = style;
      break;
    case Identifier::keyAfterColumnBorder:
      nic_->afterColumnBorder = style;
      break;
    default:
      CANNOT_HAPPEN();
    }
  }
private:
  void border(StyleObj *style, StyleObj *style2,
              void (FOTBuilder::*setter)(), ProcessContext &context) {
    FOTBuilder &fotb = context.currentFOTBuilder();
    if (!style)
      style = style2;
    if (style)
      context.currentStyleStack().push(style, context.vm(), fotb);
    (fotb.*setter)();
    if (style)
      context.currentStyleStack().pop();
  }
  Owner<NIC> nic_;
};

class TablePartFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  TablePartFlowObj() : nic_(new FOTBuilder::TablePartNIC) { }
  TablePartFlowObj(const TablePartFlowObj &fo)
    : CompoundFlowObj(fo), nic_(new FOTBuilder::TablePartNIC(*fo.nic_)) { }
  void processInner(ProcessContext &context) {
    context.startTablePart();
    FOTBuilder &fotb = context.currentFOTBuilder();
    Vector<FOTBuilder *> fotbs(2);
    fotb.startTablePart(*nic_, fotbs[0], fotbs[1]);
    Vector<SymbolObj *> labels(2);
    labels[0] = context.vm().interp->portName(Interpreter::portHeader);
    labels[1] = context.vm().interp->portName(Interpreter::portFooter);
    context.pushPorts(1, labels, fotbs);
    CompoundFlowObj::processInner(context);
    context.popPorts();
    if (context.inTableRow())
      context.endTableRow();
    context.endTablePart();
    fotb.endTablePart();
  }
  FlowObj *copy(Collector &c) const {
    return new (c) TablePartFlowObj(*this);
  }
  void setNonInheritedC(const Identifier *ident, ELObj *obj,
			const Location &loc, Interpreter &interp) {
    setDisplayNIC(*nic_, ident, obj, loc, interp);
  }
  bool hasNonInheritedC(const Identifier *ident) const {
    if (!isDisplayNIC(ident))
      return 0;
    Identifier::SyntacticKey key;
    ident->syntacticKey(key);
    if (key == Identifier::keyPositionPreference)
      return 0;
    return 1;
  }
private:
  Owner<FOTBuilder::TablePartNIC> nic_;
};

class TableColumnFlowObj : public FlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  struct NIC : public FOTBuilder::TableColumnNIC {
    NIC() : hasColumnNumber(0) { }
    bool hasColumnNumber;
  };
  TableColumnFlowObj() : nic_(new NIC) { }
  TableColumnFlowObj(const TableColumnFlowObj &fo) : FlowObj(fo), nic_(new NIC(*fo.nic_)) { }
  void processInner(ProcessContext &context) {
    if (nic_->hasColumnNumber) {
      context.currentFOTBuilder().tableColumn(*nic_);
      context.addTableColumn(nic_->columnIndex, nic_->nColumnsSpanned, style_);
    }
    else {
      FOTBuilder::TableColumnNIC nic(*nic_);
      nic.columnIndex = context.currentTableColumn();
      context.currentFOTBuilder().tableColumn(nic);
      context.addTableColumn(nic.columnIndex, nic_->nColumnsSpanned, style_);
    }
  }
  FlowObj *copy(Collector &c) const {
    return new (c) TableColumnFlowObj(*this);
  }
  bool hasNonInheritedC(const Identifier *ident) const {
    Identifier::SyntacticKey key;
    if (ident->syntacticKey(key)) {
      switch (key) {
      case Identifier::keyColumnNumber:
      case Identifier::keyNColumnsSpanned:
      case Identifier::keyWidth:
	return 1;
      default:
	break;
      }
    }
    return 0;
  }
  void setNonInheritedC(const Identifier *ident, ELObj *obj,
  			const Location &loc, Interpreter &interp) {
    Identifier::SyntacticKey key;
    if (ident->syntacticKey(key)) {
      switch (key) {
      case Identifier::keyColumnNumber:
      case Identifier::keyNColumnsSpanned:
	{
	  long n;
	  if (!interp.convertIntegerC(obj, ident, loc, n))
	    return;
          if (n <= 0) {
	    interp.setNextLocation(loc);
            interp.message(InterpreterMessages::invalidCharacteristicValue,
	  		   StringMessageArg(ident->name()));
	    return;
          }
	  if (key == Identifier::keyColumnNumber) {
	    nic_->columnIndex = n - 1;
	    nic_->hasColumnNumber = 1;
	  }
	  else
	    nic_->nColumnsSpanned = n;
	}
	return;
      case Identifier::keyWidth:
	{
	  const LengthSpec *ls = obj->lengthSpec();
	  if (ls) {
	    // width is a TableLengthSpec not just a LengthSpec
	    if (ls->convert(nic_->width))
	      nic_->hasWidth = 1;
	    else
	      interp.invalidCharacteristicValue(ident, loc);
	  }
	  else if (interp.convertLengthSpecC(obj, ident, loc, nic_->width))
	    nic_->hasWidth = 1;
	}
	return;
      default:
	break;
      }
    }
    CANNOT_HAPPEN();
  }
private:
  Owner<NIC> nic_;
};

class TableRowFlowObj : public CompoundFlowObj {
public:
  TableRowFlowObj() { }
  void pushStyle(ProcessContext &, unsigned &) {  }
  void popStyle(ProcessContext &, unsigned) {  }
  void processInner(ProcessContext &context) {
    if (!context.inTable()) {
      // FIXME location
      context.vm().interp->message(InterpreterMessages::tableRowOutsideTable);
      CompoundFlowObj::processInner(context);
      return;
    }
    if (context.inTableRow())
      context.endTableRow();
    context.startTableRow(style_);
    CompoundFlowObj::processInner(context);
    if (context.inTableRow())
      context.endTableRow();
    // else FIXME give an error
    // must have used ends-row? in a table-row
  }
  FlowObj *copy(Collector &c) const {
    return new (c) TableRowFlowObj(*this);
  }
};

class TableCellFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  struct NIC : FOTBuilder::TableCellNIC {
    NIC() : startsRow(0), endsRow(0), hasColumnNumber(0) { }
    bool hasColumnNumber;
    bool startsRow;
    bool endsRow;
  };
  TableCellFlowObj(bool missing = 0) : nic_(new NIC) {
    if (missing)
      nic_->missing = 1;
  }
  TableCellFlowObj(const TableCellFlowObj &fo) : CompoundFlowObj(fo), nic_(new NIC(*fo.nic_)) { }
  void pushStyle(ProcessContext &context, unsigned &nPush) {
    if (context.inTableRow()) {
      if (nic_->startsRow) {
	context.endTableRow();
	context.startTableRow(0);
      }
    }
    else
      context.startTableRow(0);
    unsigned columnNumber
      = nic_->hasColumnNumber ? nic_->columnIndex : context.currentTableColumn();
    StyleObj *columnStyle = context.tableColumnStyle(columnNumber, nic_->nColumnsSpanned);
    if (columnStyle) {
      context.currentStyleStack().push(columnStyle, context.vm(), context.currentFOTBuilder());
      context.currentFOTBuilder().startSequence();
      nPush++;
    }
    StyleObj *rowStyle = context.tableRowStyle();
    if (rowStyle) {
      context.currentStyleStack().push(rowStyle, context.vm(), context.currentFOTBuilder());
      context.currentFOTBuilder().startSequence();
      nPush++;
    }
    CompoundFlowObj::pushStyle(context, nPush);
  }
  void popStyle(ProcessContext &context, unsigned nPush) {
    CompoundFlowObj::popStyle(context, nPush);
    for (unsigned i = 0; i < nPush; i++) {
      context.currentFOTBuilder().endSequence();
      context.currentStyleStack().pop();
    }
    if (nic_->endsRow)
      context.endTableRow();
  }
  void processInner(ProcessContext &context) {
    if (!context.inTable()) {
      // FIXME location
      context.vm().interp->message(InterpreterMessages::tableCellOutsideTable);
      CompoundFlowObj::processInner(context);
      return;
    }
    FOTBuilder &fotb = context.currentFOTBuilder();
    if (!nic_->hasColumnNumber) {
      FOTBuilder::TableCellNIC nic(*nic_);
      nic.columnIndex = context.currentTableColumn();
      fotb.startTableCell(nic);
      if (!nic_->missing)
	context.noteTableCell(nic.columnIndex, nic.nColumnsSpanned, nic.nRowsSpanned);
    }
    else {
      fotb.startTableCell(*nic_);
      if (!nic_->missing)
	context.noteTableCell(nic_->columnIndex, nic_->nColumnsSpanned, nic_->nRowsSpanned);
    }
    Interpreter &interp = *context.vm().interp;
    border(interp.cellBeforeRowBorderC(), &FOTBuilder::tableCellBeforeRowBorder, context);
    border(interp.cellAfterRowBorderC(), &FOTBuilder::tableCellAfterRowBorder, context);
    border(interp.cellBeforeColumnBorderC(), &FOTBuilder::tableCellBeforeColumnBorder, context);
    border(interp.cellAfterColumnBorderC(), &FOTBuilder::tableCellAfterColumnBorder, context);
    CompoundFlowObj::processInner(context);
    fotb.endTableCell();
  }
  FlowObj *copy(Collector &c) const {
    return new (c) TableCellFlowObj(*this);
  }
  bool hasNonInheritedC(const Identifier *ident) const {
    Identifier::SyntacticKey key;
    if (ident->syntacticKey(key)) {
      switch (key) {
      case Identifier::keyNRowsSpanned:
	return 1;
      default:
	break;
      }
    }
    return 0;
  }
  bool hasPseudoNonInheritedC(const Identifier *ident) const {
    Identifier::SyntacticKey key;
    if (ident->syntacticKey(key)) {
      switch (key) {
      case Identifier::keyColumnNumber:
      case Identifier::keyNColumnsSpanned:
      case Identifier::keyIsStartsRow:
      case Identifier::keyIsEndsRow:
	return 1;
      default:
	break;
      }
    }
    return 0;
  }
  void setNonInheritedC(const Identifier *ident, ELObj *obj,
  			const Location &loc, Interpreter &interp) {
    Identifier::SyntacticKey key;
    if (ident->syntacticKey(key)) {
      switch (key) {
      case Identifier::keyIsStartsRow:
        interp.convertBooleanC(obj, ident, loc, nic_->startsRow);
	return;
      case Identifier::keyIsEndsRow:
        interp.convertBooleanC(obj, ident, loc, nic_->endsRow);
	return;
      case Identifier::keyColumnNumber:
      case Identifier::keyNColumnsSpanned:
      case Identifier::keyNRowsSpanned:
	{
	  long n;
          if (!interp.convertIntegerC(obj, ident, loc, n))
	    return;
          if (n <= 0) {
	    interp.setNextLocation(loc);
	    interp.message(InterpreterMessages::invalidCharacteristicValue,
  		 	   StringMessageArg(ident->name()));
	    return;
          }
	  if (key == Identifier::keyColumnNumber) {
	    nic_->columnIndex = n - 1;
	    nic_->hasColumnNumber = 1;
	  }
	  else if (key == Identifier::keyNColumnsSpanned)
	    nic_->nColumnsSpanned = n;
	  else
	    nic_->nRowsSpanned = n;
	}
	return;
      default:
	break;
      }
    }
    CANNOT_HAPPEN();
  }
private:
  void border(const ConstPtr<InheritedC> &ic, void (FOTBuilder::*setter)(),
	      ProcessContext &context) {
    Interpreter &interp = *context.vm().interp;
    Vector<size_t> dep;
    ELObj *obj = context.currentStyleStack().actual(ic, interp, dep);
    StyleObj *style;
    if (obj == interp.makeFalse())
      style = interp.borderFalseStyle();
    else if (obj == interp.makeTrue())
      style = interp.borderTrueStyle();
    else {
      SosofoObj *sosofo = obj->asSosofo();
      if (!sosofo || !sosofo->tableBorderStyle(style))
	style = 0;
    }
    FOTBuilder &fotb = context.currentFOTBuilder();
    if (style)
      context.currentStyleStack().push(style, context.vm(), fotb);
    (fotb.*setter)();
    if (style)
      context.currentStyleStack().pop();
  }
  Owner<NIC> nic_;
};

class TableBorderFlowObj : public FlowObj {
public:
  TableBorderFlowObj() { }
  void process(ProcessContext &) { }
  void processInner(ProcessContext &) { }
  bool tableBorderStyle(StyleObj *&style) {
    style = style_;
    return 1;
  }
  FlowObj *copy(Collector &c) const {
    return new (c) TableBorderFlowObj(*this);
  }
};


void ProcessContext::startTable()
{
  tableStack_.insert(new Table);
}

void ProcessContext::endTable()
{
  coverSpannedRows();
  delete tableStack_.get();
}

void ProcessContext::coverSpannedRows()
{
  // Generate empty cells to cover any remaining vertical spans
  Table *table = tableStack_.head();
  if (!table)
    return;
  unsigned n = 0;
  for (size_t i = 0; i < table->covered.size(); i++)
    if (table->covered[i] > n)
      n = table->covered[i];
  for (; n > 0; n--) {
    SosofoObj *content = new (*vm().interp) EmptySosofoObj;
    ELObjDynamicRoot protect(*vm().interp, content);
    TableRowFlowObj *row = new (*vm().interp) TableRowFlowObj;
    row->setContent(content);
    protect = row;
    row->process(*this);
  }
}

void ProcessContext::startTablePart()
{
  Table *table = tableStack_.head();
  if (table) {
    table->currentColumn = 0;
    table->rowStyle = 0;
    table->columnStyles.clear();
    table->covered.clear();
    table->nColumns = 0;
  }
}

void ProcessContext::endTablePart()
{
  coverSpannedRows();
}

void ProcessContext::addTableColumn(unsigned columnIndex, unsigned span, StyleObj *style)
{
  Table *table = tableStack_.head();
  if (table) {
    table->currentColumn = columnIndex + span;
    if (columnIndex >= table->columnStyles.size())
      table->columnStyles.resize(columnIndex + 1);
    Vector<StyleObj *> &tem = table->columnStyles[columnIndex];
    if (span > 0) {
      while (tem.size() < span)
	tem.push_back((StyleObj *)0);
      tem[span - 1] = style;
    }
  }
}

unsigned ProcessContext::currentTableColumn()
{
  Table *table = tableStack_.head();
  if (table)
    return table->currentColumn;
  return 0;
}

void ProcessContext::noteTableCell(unsigned colIndex, unsigned colSpan, unsigned rowSpan)
{
  Table *table = tableStack_.head();
  if (!table)
    return;
  table->currentColumn = colIndex + colSpan;
  Vector<unsigned> &covered = table->covered;
  for (size_t i = covered.size(); i < colIndex + colSpan; i++)
    covered.push_back(0);
  for (size_t i = 0; i < colSpan; i++)
    covered[colIndex + i] = rowSpan;
  if (colIndex + colSpan > table->nColumns)
    table->nColumns = colIndex + colSpan;
}

StyleObj *ProcessContext::tableColumnStyle(unsigned columnIndex, unsigned span)
{
  Table *table = tableStack_.head();
  if (table) {
    if (columnIndex < table->columnStyles.size()) {
      Vector<StyleObj *> &tem = table->columnStyles[columnIndex];
      if (span > 0 && span <= tem.size())
	return tem[span - 1];
    }
  }
  return 0;
}
 
StyleObj *ProcessContext::tableRowStyle()
{
  Table *table = tableStack_.head();
  if (table)
    return table->rowStyle;
  return 0;
}

void ProcessContext::startTableRow(StyleObj *style)
{
  Table *table = tableStack_.head();
  if (table) {
    table->rowStyle = style;
    table->currentColumn = 0;
    table->inTableRow = 1;
    table->rowConnectableLevel = connectionStack_.head()->connectableLevel;
  }
  currentFOTBuilder().startTableRow();
}

void ProcessContext::endTableRow()
{
  Table *table = tableStack_.head();
  if (table) {
    // Fill in blank cells
    Vector<unsigned> &covered = table->covered;
    for (size_t i = 0; i < table->nColumns + 1; i++) {
      if (i >= covered.size() || !covered[i]) {
	table->currentColumn = i;
	SosofoObj *content = new (*vm().interp) EmptySosofoObj;
	ELObjDynamicRoot protect(*vm().interp, content);
	// The last cell is a dummy one
	TableCellFlowObj *cell = new (*vm().interp) TableCellFlowObj(i >= table->nColumns);
	cell->setContent(content);
	protect = cell;
	cell->process(*this);
      }
      // cell->process() will cover it
      if (i < table->nColumns)
	covered[i] -= 1;
    }
    table->inTableRow = 0;
  }
  currentFOTBuilder().endTableRow();
}

bool ProcessContext::inTableRow()
{
  Table *table = tableStack_.head();
  if (table)
    return table->inTableRow;
  return 0;
}

ProcessContext::Table::Table()
: rowStyle(0), currentColumn(0), inTableRow(0), nColumns(0)
{
}


// Flow object classes declared with declare-flow-object-class
// that we don't know about are assumed to have one principal port
// and accept any non-inherited characteristic.

class UnknownFlowObj : public CompoundFlowObj {
public:
  UnknownFlowObj() { }
  FlowObj *copy(Collector &c) const {
    return new (c) UnknownFlowObj(*this);
  }
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &) { }
  bool hasNonInheritedC(const Identifier *ident) const {
    Identifier::SyntacticKey syn;
    if (ident->syntacticKey(syn)
        && (syn == Identifier::keyLabel || syn == Identifier::keyContentMap))
      return 0;
    if (!ident->inheritedC().isNull())
      return 0;
    return 1;
  }
};

class FormattingInstructionFlowObj : public FlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  FormattingInstructionFlowObj() { }
  void processInner(ProcessContext &context) {
    context.currentFOTBuilder().formattingInstruction(data_);
  }
  FlowObj *copy(Collector &c) const {
    return new (c) FormattingInstructionFlowObj(*this);
  }
  void setNonInheritedC(const Identifier *ident, ELObj *obj,
			const Location &loc, Interpreter &interp) {
    interp.convertStringC(obj, ident, loc, data_);
  }
  bool hasNonInheritedC(const Identifier *ident) const {
    Identifier::SyntacticKey key;
    return ident->syntacticKey(key) && key == Identifier::keyData;
  }
private:
  StringC data_;
};

class ELObjExtensionFlowObjValue : public FOTBuilder::ExtensionFlowObj::Value {
public:
  ELObjExtensionFlowObjValue(const Identifier *ident, ELObj *obj,
			     const Location &loc, Interpreter &interp)
  : ident_(ident), obj_(obj), loc_(&loc), interp_(&interp) { }
  bool convertString(StringC &result) const {
    return interp_->convertStringC(obj_, ident_, *loc_, result);
  }
  bool convertStringPairList(Vector<StringC> &v) const {
    ELObj *obj = obj_;
    for (;;) {
      if (obj->isNil())
	return 1;	
      PairObj *pair = obj->asPair();
      if (!pair)
	break;
      obj = pair->cdr();
      PairObj *att = pair->car()->asPair();
      if (!att)
	break;
      const Char *s;
      size_t n;
      if (!att->car()->stringData(s, n))
	break;
      v.resize(v.size() + 1);
      v.back().assign(s, n);
      att = att->cdr()->asPair();
      if (!att || !att->car()->stringData(s, n) || !att->cdr()->isNil()) {
	v.resize(v.size() - 1);
	break;
      }
      v.resize(v.size() + 1);
      v.back().assign(s, n);
    }
    interp_->setNextLocation(*loc_);
    interp_->message(InterpreterMessages::invalidCharacteristicValue,
		     StringMessageArg(ident_->name()));
    return 0;
  }
  bool convertStringList(Vector<StringC> &v) const {
    ELObj *obj = obj_;
    for (;;) {
      if (obj->isNil())
        return 1;       
      PairObj *pair = obj->asPair();
      if (!pair)
        break;
      const Char *s;
      size_t n;
      if (!pair->car()->stringData(s, n))
        break;
      v.resize(v.size() + 1);
      v.back().assign(s, n);
      obj = pair->cdr();
    }
    interp_->setNextLocation(*loc_);
    interp_->message(InterpreterMessages::invalidCharacteristicValue,
                     StringMessageArg(ident_->name()));
    return 0;
  } 
  bool convertBoolean(bool &result) const {
    return interp_->convertBooleanC(obj_, ident_, *loc_, result);
  }
private:
  ELObj *obj_;
  const Identifier *ident_;
  const Location *loc_;
  Interpreter *interp_;
};

class ExtensionFlowObj : public FlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  ExtensionFlowObj(const FOTBuilder::ExtensionFlowObj &);
  ExtensionFlowObj(const ExtensionFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  bool hasNonInheritedC(const Identifier *) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
private:
  Owner<FOTBuilder::ExtensionFlowObj> fo_;
};

ExtensionFlowObj::ExtensionFlowObj(const FOTBuilder::ExtensionFlowObj &fo)
: fo_(fo.copy())
{
}

ExtensionFlowObj::ExtensionFlowObj(const ExtensionFlowObj &fo)
: FlowObj(fo), fo_(fo.fo_->copy())
{
}

void ExtensionFlowObj::processInner(ProcessContext &context)
{
  context.currentFOTBuilder().extension(*fo_, context.vm().currentNode);
}

void ExtensionFlowObj::setNonInheritedC(const Identifier *ident,
					ELObj *obj,
					const Location &loc,
					Interpreter &interp)
{
  fo_->setNIC(ident->name(), ELObjExtensionFlowObjValue(ident, obj, loc, interp));
}

bool ExtensionFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  return fo_->hasNIC(ident->name());
}

FlowObj *ExtensionFlowObj::copy(Collector &c) const
{
  return new (c) ExtensionFlowObj(*this);
}

class CompoundExtensionFlowObj : public CompoundFlowObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  CompoundExtensionFlowObj(const FOTBuilder::CompoundExtensionFlowObj &);
  CompoundExtensionFlowObj(const CompoundExtensionFlowObj &);
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
  bool hasNonInheritedC(const Identifier *) const;
  void setNonInheritedC(const Identifier *, ELObj *,
			const Location &, Interpreter &);
private:
  Owner<FOTBuilder::CompoundExtensionFlowObj> fo_;
};

CompoundExtensionFlowObj::CompoundExtensionFlowObj(const FOTBuilder::CompoundExtensionFlowObj &fo)
: fo_(fo.copy()->asCompoundExtensionFlowObj())
{
}

CompoundExtensionFlowObj::CompoundExtensionFlowObj(const CompoundExtensionFlowObj &fo)
: CompoundFlowObj(fo), fo_(fo.fo_->copy()->asCompoundExtensionFlowObj())
{
}

void CompoundExtensionFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  Vector<StringC> portNames;
  fo_->portNames(portNames);
  Vector<FOTBuilder *> fotbs(portNames.size());
  fotb.startExtension(*fo_, context.vm().currentNode, fotbs);
  if (portNames.size()) {
    Vector<SymbolObj *> portSyms(portNames.size());
    for (size_t i = 0; i < portSyms.size(); i++)
      portSyms[i] = context.vm().interp->makeSymbol(portNames[i]);
    context.pushPorts(fo_->hasPrincipalPort(), portSyms, fotbs);
    CompoundFlowObj::processInner(context);
    context.popPorts();
  }
  else
    CompoundFlowObj::processInner(context);
  fotb.endExtension(*fo_);
}

void CompoundExtensionFlowObj::setNonInheritedC(const Identifier *ident,
					        ELObj *obj,
						const Location &loc,
						Interpreter &interp)
{
  fo_->setNIC(ident->name(), ELObjExtensionFlowObjValue(ident, obj, loc, interp));
}

bool CompoundExtensionFlowObj::hasNonInheritedC(const Identifier *ident) const
{
  return fo_->hasNIC(ident->name());
}

FlowObj *CompoundExtensionFlowObj::copy(Collector &c) const
{
  return new (c) CompoundExtensionFlowObj(*this);
}

#define FLOW_OBJ(name, string) \
{ FlowObj *tem = new (*this) name; \
  lookup(makeStringC(string))->setFlowObj(tem); \
  makePermanent(tem); }

void Interpreter::installFlowObjs()
{
  FLOW_OBJ(SequenceFlowObj, "sequence");
  FLOW_OBJ(DisplayGroupFlowObj, "display-group");
  FLOW_OBJ(ParagraphFlowObj, "paragraph");
  FLOW_OBJ(ParagraphBreakFlowObj, "paragraph-break");
  FLOW_OBJ(LineFieldFlowObj, "line-field");
  FLOW_OBJ(ScoreFlowObj, "score");
  FLOW_OBJ(ExternalGraphicFlowObj, "external-graphic");
  FLOW_OBJ(RuleFlowObj, "rule");
  FLOW_OBJ(LeaderFlowObj, "leader");
  FLOW_OBJ(CharacterFlowObj, "character");
  FLOW_OBJ(BoxFlowObj, "box");
  FLOW_OBJ(AlignmentPointFlowObj, "alignment-point");
  FLOW_OBJ(SidelineFlowObj, "sideline");
  // simple-page
  FLOW_OBJ(SimplePageSequenceFlowObj, "simple-page-sequence");
  // tables
  FLOW_OBJ(TableFlowObj, "table");
  FLOW_OBJ(TablePartFlowObj, "table-part");
  FLOW_OBJ(TableColumnFlowObj, "table-column");
  FLOW_OBJ(TableRowFlowObj, "table-row");
  FLOW_OBJ(TableCellFlowObj, "table-cell");
  FLOW_OBJ(TableBorderFlowObj, "table-border");
  // online
  FLOW_OBJ(LinkFlowObj, "link");
  FLOW_OBJ(ScrollFlowObj, "scroll");
  FLOW_OBJ(MarginaliaFlowObj, "marginalia");
  FLOW_OBJ(MultiModeFlowObj, "multi-mode");
  // math
  FLOW_OBJ(MathSequenceFlowObj, "math-sequence");
  FLOW_OBJ(FractionFlowObj, "fraction");
  FLOW_OBJ(UnmathFlowObj, "unmath");
  FLOW_OBJ(SuperscriptFlowObj, "superscript");
  FLOW_OBJ(SubscriptFlowObj, "subscript");
  FLOW_OBJ(ScriptFlowObj, "script");
  FLOW_OBJ(MarkFlowObj, "mark");
  FLOW_OBJ(FenceFlowObj, "fence");
  FLOW_OBJ(RadicalFlowObj, "radical");
  FLOW_OBJ(MathOperatorFlowObj, "math-operator");
  FLOW_OBJ(GridFlowObj, "grid");
  FLOW_OBJ(GridCellFlowObj, "grid-cell");
}

void Interpreter::installExtensionFlowObjectClass(Identifier *ident,
						  const StringC &pubid,
						  const Location &loc)
{
  FlowObj *tem = 0;
  if (extensionTable_) {
    for (const FOTBuilder::Extension *ep = extensionTable_; ep->pubid; ep++) {
      if (pubid == ep->pubid) {
	if (ep->flowObj) {
	  const FOTBuilder::CompoundExtensionFlowObj *cFlowObj
	    = ep->flowObj->asCompoundExtensionFlowObj();
	  if (cFlowObj)
	    tem = new (*this) CompoundExtensionFlowObj(*cFlowObj);
	  else
	    tem = new (*this) ExtensionFlowObj(*ep->flowObj);
	}
	break;
      }
    }
  }
  if (!tem) {
    if (pubid
        == "UNREGISTERED::James Clark//Flow Object Class::"
           "formatting-instruction")
      tem = new (*this) FormattingInstructionFlowObj;
    else
      tem = new (*this) UnknownFlowObj;
  }
  makePermanent(tem);
  ident->setFlowObj(tem, currentPartIndex(), loc);
}

#ifdef DSSSL_NAMESPACE
}
#endif

#include "FlowObj_inst.cxx"
