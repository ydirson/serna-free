// 
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
// 
// This file is part of Syntext Serna XML Editor.
// 
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
// 
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General 
// Public License versions 2.0 or 3.0 as published by the Free Software 
// Foundation and appearing in the file LICENSE.GPL included in the packaging 
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL 
// Exception for Syntext Serna Free Edition, included in the file 
// GPL_EXCEPTION.txt in this package.
// 
// You should have received a copy of appropriate licenses along with this 
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales 
// department at sales@syntext.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// 
#include "ui/ui_defs.h"
#include "ui/PropertyEditor.h"

#include <qtpropertymanager.h>
#include <qteditorfactory.h>

#include <QMap>
#include <QIcon>
#include <QComboBox>
#include <QLineEdit>
#include <QValidator>
#include <QAbstractItemView>
#include <QApplication>

using namespace Common;

namespace {

class EnumPropertyManager : public QtEnumPropertyManager {
public:
    EnumPropertyManager(QObject* parent)
        : QtEnumPropertyManager(parent) {}

    virtual QIcon valueIcon(const QtProperty* qp) const
    {
        Sui::PropertyEditor& pe = static_cast<Sui::PropertyEditor&>(*parent());
        const PropertyNode* pn = pe.qpropValue(qp);
        return pn ? pe.valueIcon(pn, valueText(qp))
            : QIcon();
    }
};

class EnumEditorFactory : public QtEnumEditorFactory {
public:
    EnumEditorFactory(QObject* parent)
        : QtEnumEditorFactory(parent) {}
    virtual QWidget* createEditor(QtEnumPropertyManager* m,
                                  QtProperty* qp, QWidget* pwidget)       
    {
        QComboBox* cb = dynamic_cast<QComboBox*>
            (QtEnumEditorFactory::createEditor(m, qp, pwidget));
        Sui::PropertyEditor& pe = static_cast<Sui::PropertyEditor&>(*parent());
        QtEnumPropertyManager* epm = 
            dynamic_cast<QtEnumPropertyManager*>(qp->propertyManager());
        const PropertyNode* pn = pe.qpropValue(qp);
        if (0 == epm || 0 == pn)
            return cb;
        QStringList icon_names = epm->enumNames(qp);
        for (int i = 0; i < icon_names.size(); ++i) 
            cb->setItemIcon(i, pe.valueIcon(pn, icon_names[i]));
        cb->view()->setTextElideMode(Qt::ElideRight);
        return cb;
    }
};

class LineEditFactory : public QtAbstractEditorFactory<QtStringPropertyManager>
{
    Q_OBJECT
public:
    LineEditFactory(QObject *parent = 0);
    ~LineEditFactory();
protected:
    virtual void connectPropertyManager(QtStringPropertyManager *manager);
    virtual QWidget *createEditor(QtStringPropertyManager *manager, 
                                  QtProperty *property, QWidget *parent);
    virtual void disconnectPropertyManager(QtStringPropertyManager *manager);

public slots:
    void    slotPropertyChanged(QtProperty*, const QString&);
    void    slotEditorDestroyed(QObject *);
    void    editingFinished();

private:
    Q_DISABLE_COPY(LineEditFactory)
    QMap<QtProperty*, QList<QLineEdit*> > m_createdEditors;
    QMap<QLineEdit*,   QtProperty*> m_editorToProperty;
};

} // namespace

namespace Sui {

class PropertyEditor::PropertyManagerSet : 
    public QMap<QString, QtAbstractPropertyManager*> {};
class PropertyEditor::QPropToNodeMap :
    public QMap<const QtProperty*, PropertyNode*> {};
class PropertyEditor::NodeToQPropMap :
    public QMap<const PropertyNode*, QtProperty*> {};

#define REGISTER_EDITOR(M, E) add_manager<M, E>(#M)

template <class MC, class FC> 
void PropertyEditor::add_manager(const String& name)
{
    MC* pm = new MC(this);        
    setFactoryForManager(pm, new FC(this));
    propertyManagers_->insert(name, pm);
    connect(pm, SIGNAL(propertyChanged(QtProperty*)), this, 
        SLOT(value_changed(QtProperty*)));
}

PropertyEditor::PropertyEditor(QWidget* parent)
    : QtTreePropertyBrowser(parent),
      propertyManagers_(new PropertyManagerSet),
      nodeMap_(new QPropToNodeMap),
      qpropMap_(new NodeToQPropMap),
      valueLock_(true)
{
    REGISTER_EDITOR(QtBoolPropertyManager, QtCheckBoxFactory);
    REGISTER_EDITOR(QtIntPropertyManager, QtSpinBoxFactory);
    QtEnumPropertyManager* epm = new EnumPropertyManager(this);
    setFactoryForManager(epm, new EnumEditorFactory(this));
    propertyManagers_->insert(NOTR("QtEnumPropertyManager"), epm);
    connect(epm, SIGNAL(propertyChanged(QtProperty*)), this, 
        SLOT(value_changed(QtProperty*)));
    add_manager<QtStringPropertyManager, LineEditFactory>
        (NOTR("QtStringPropertyManager"));
    propertyManagers_->insert(NOTR("QtGroupPropertyManager"),
        new QtGroupPropertyManager(this));
    setRootIsDecorated(false);
    setResizeMode(Interactive);
    setSplitterPosition(width());
}

QIcon PropertyEditor::valueIcon(const PropertyNode*, const String&) const
{
    return QIcon();
}

PropertyNode* PropertyEditor::qpropValue(const QtProperty* prop) const
{
    return nodeMap_->value(prop); 
}

void PropertyEditor::propertyChanged(Common::PropertyNode* pn)
{
    const PropertyNode* p = pn->parent();
    for (; p != root_; p = p->parent())
        ;
    if (0 == p) // property does not belong here
        return;
    // todo: implement property add/removal check
    valueLock_ = true;
    QtProperty* qtp = qpropMap_->value(pn);
    if (qtp)
        setPropValue(qtp, pn);
    valueLock_ = false;
}

void PropertyEditor::valueChanged(QtProperty* qtp) 
{
    PropertyNode* pn = qpropValue(qtp);
    if (0 == pn)
        return;
    pn->setString(qtp->valueText());
}

void PropertyEditor::value_changed(QtProperty* qtp)
{
    if (valueLock_)
        return;
    valueChanged(qtp);
}

void PropertyEditor::set_watchers(PropertyNode* pn)
{
    pn->addWatcher(this);
    for (pn = pn->firstChild(); pn; pn = pn->nextSibling())
        set_watchers(pn);
}

void PropertyEditor::init(PropertyNode* root, bool use_top)
{
    clear();
    root_ = root;
    deregister();
    valueLock_ = true;
    PropertyManagerSet::iterator pmi = propertyManagers_->begin();
    for (; pmi != propertyManagers_->end(); ++pmi)
        (*pmi)->clear();
    if (0 == root) {
        valueLock_ = false;
        return;
    }
    QtProperty* qtp = 0;
    if (use_top) { 
        qtp = build_properties(root, 0);
        set_watchers(root);
        if (qtp)
            addProperty(qtp);
    } else {
        for (root = root->firstChild(); root; root = root->nextSibling()) {
            qtp = build_properties(root, 0);
            if (qtp)
                addProperty(qtp);
            set_watchers(root);
        }
    }
    valueLock_ = false;
}

QtProperty*
PropertyEditor::build_properties(PropertyNode* prop, QtProperty* qpprop)
{
    QtAbstractPropertyManager* pm = managerForProperty(prop);
    if (0 == pm || prop->name().left(1) == "#")
        return 0; // do nothing for unknown properties
    QtProperty* qprop = pm->addProperty(prop->name());
    if (qpprop)
        qpprop->addSubProperty(qprop);
    setPropValue(qprop, prop);
    for (prop = prop->firstChild(); prop; prop = prop->nextSibling())
        build_properties(prop, qprop);
    return qprop;
}

void PropertyEditor::setPropValue(QtProperty* qp, const PropertyNode* p)
{
    QtAbstractPropertyManager* pm = qp->propertyManager();
    qp->setToolTip(QString(NOTR("<qt><b>%0</b>: %1<br/><b>%2</b>: %3")).
        arg(qApp->translate("QtTreePropertyBrowser", "Property")).
        arg(p->name()).
        arg(qApp->translate("QtTreePropertyBrowser", "Value")).
        arg(p->getString()));
    if (pm->inherits(NOTR("QtStringPropertyManager"))) 
        static_cast<QtStringPropertyManager*>(pm)->setValue(qp, p->getString());
    else if (pm->inherits(NOTR("QtBoolPropertyManager"))) 
        static_cast<QtBoolPropertyManager*>(pm)->setValue(qp, p->getBool());
    else if (pm->inherits(NOTR("QtIntPropertyManager")))
        static_cast<QtIntPropertyManager*>(pm)->setValue(qp, p->getInt());
    else if (pm->inherits(NOTR("QtEnumPropertyManager"))) {
        QtEnumPropertyManager* epm = static_cast<QtEnumPropertyManager*>(pm);
        QStringList enum_list = epm->enumNames(qp);
        int i = enum_list.indexOf(p->getString());
        if (i < 0)
            i = 0;
        if (i < enum_list.size())
            epm->setValue(qp, i);
    }
    nodeMap_->insert(qp, const_cast<PropertyNode*>(p));
    qpropMap_->insert(p, qp);
}
    
QtAbstractPropertyManager* 
PropertyEditor::managerForProperty(const Common::PropertyNode*) const
{
    return managerForName(NOTR("QtStringPropertyManager"));
}

QtAbstractPropertyManager* 
PropertyEditor::managerForName(const Common::String& name) const
{
    return propertyManagers_->value(name);
}

PropertyEditor::~PropertyEditor()
{
}
 
} // namespace Sui

// implementation of custom line edit factory

LineEditFactory::LineEditFactory(QObject* parent)
    : QtAbstractEditorFactory<QtStringPropertyManager>(parent) 
{
}

void LineEditFactory::slotPropertyChanged(QtProperty *property,
                                          const QString &value)
{
    if (!m_createdEditors.contains(property))
        return;
    QList<QLineEdit *> editors = m_createdEditors[property];
    QListIterator<QLineEdit *> itEditor(editors);
    while (itEditor.hasNext()) {
        QLineEdit *editor = itEditor.next();
        if (editor->text() != value)
            editor->setText(value);
    }
}

void LineEditFactory::editingFinished()
{
    QObject *object = sender();
    QMap<QLineEdit *, QtProperty *>::ConstIterator itEditor =
                m_editorToProperty.constBegin();
    while (itEditor != m_editorToProperty.constEnd()) {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtStringPropertyManager *manager = propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, itEditor.key()->text());
            return;
        }
        itEditor++;
    }
}

void LineEditFactory::slotEditorDestroyed(QObject *object)
{
    QMap<QLineEdit *, QtProperty *>::ConstIterator itEditor =
                m_editorToProperty.constBegin();
    while (itEditor != m_editorToProperty.constEnd()) {
        if (itEditor.key() == object) {
            QLineEdit *editor = itEditor.key();
            QtProperty *property = itEditor.value();
            m_editorToProperty.remove(editor);
            m_createdEditors[property].removeAll(editor);
            if (m_createdEditors[property].isEmpty())
                m_createdEditors.remove(property);
            return;
        }
        itEditor++;
    }
}

LineEditFactory::~LineEditFactory()
{
    QMap<QLineEdit*, QtProperty*> editorToProperty = m_editorToProperty;
    QMap<QLineEdit*, QtProperty*>::ConstIterator it = 
        editorToProperty.constBegin();
    while (it != editorToProperty.constEnd()) {
        delete it.key();
        it++;
    }
}

void LineEditFactory::connectPropertyManager(QtStringPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty*, const QString&)),
                this, SLOT(slotPropertyChanged(QtProperty*, const QString&)));
}

QWidget *LineEditFactory::createEditor(QtStringPropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    QLineEdit *editor = new QLineEdit(parent);
    QRegExp regExp = manager->regExp(property);
    if (regExp.isValid()) {
        QValidator *validator = new QRegExpValidator(regExp, editor);
        editor->setValidator(validator);
    }
    editor->setText(manager->value(property));
    m_createdEditors[property].append(editor);
    m_editorToProperty[editor] = property;
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    connect(editor, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    return editor;
}

void 
LineEditFactory::disconnectPropertyManager(QtStringPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty*, const QString&)),
                this, SLOT(slotPropertyChanged(QtProperty*, const QString &)));
}

#include "moc/PropertyEditor.moc"
