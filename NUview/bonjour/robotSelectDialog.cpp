#include <QtGui>
#include "robotSelectDialog.h"
#include "bonjourservicebrowser.h"
#include "bonjourserviceresolver.h"
#include <QDebug>

robotSelectDialog::robotSelectDialog(QWidget * parent, const QString& service): QDialog(parent)
{
    bonjourBrowser = new BonjourServiceBrowser(this);
    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabels(QStringList() << tr("Available NUbot Robots"));

    connect(bonjourBrowser, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
                this, SLOT(updateRecords(const QList<BonjourRecord> &)));

    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);
    connectButton->setEnabled(false);

    cancelButton = new QPushButton(tr("Cancel"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(connectButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveSelected()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(treeWidget, 0, 0, 2, 2);
//    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Robot Selection"));
    treeWidget->setFocus();
    bonjourBrowser->browseForServiceType(service);
}

void robotSelectDialog::saveSelected()
{
    QList<QTreeWidgetItem *> selectedItems = treeWidget->selectedItems();
    if(selectedItems.length() > 0)
    {
        QTreeWidgetItem *item = selectedItems.at(0);
        QVariant variant = item->data(0, Qt::UserRole);
        m_selectedHost = variant.value<BonjourRecord>();
    }
    else
    {
        m_selectedHost = BonjourRecord();
    }
}


void robotSelectDialog::enableConnectButton()
{
    connectButton->setEnabled(treeWidget->invisibleRootItem()->childCount() != 0);
}

void robotSelectDialog::updateRecords(const QList<BonjourRecord> &list)
{
    treeWidget->clear();
    foreach (BonjourRecord record, list) {
        QVariant variant;
        variant.setValue(record);
        QTreeWidgetItem *processItem = new QTreeWidgetItem(treeWidget,
                                                           QStringList() << record.serviceName);
        processItem->setData(0, Qt::UserRole, variant);
    }

    if (treeWidget->invisibleRootItem()->childCount() > 0) {
        treeWidget->invisibleRootItem()->child(0)->setSelected(true);
    }
    enableConnectButton();
}
