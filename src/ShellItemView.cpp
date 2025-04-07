#include "ShellItemView.h"
#include "ShellCompletionItem.h"

#include <QListView>
#include <QItemSelectionModel>
#include <qobject.h>

ShellItemView::ShellItemView():
    QListView()
{
    init();
}

void ShellItemView::init()
{
    setMouseTracking(true);
    m_infoPopup = new QWidget();
    m_infoPopup->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    m_infoLabel = new QLabel(m_infoPopup);

    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &ShellItemView::onSelectionChange);
}

void ShellItemView::mouseMoveEvent(QMouseEvent* event)
{
    QModelIndex index { indexAt(viewport()->mapFrom(this, event->pos())) };
    selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    QListView::mouseMoveEvent(event);
}

void ShellItemView::onSelectionChange(QItemSelection& selected, QItemSelection& deselected)
{
    // There's only ever going to be 1 item selected, so get it
    showInfoPopup(&selected.first().indexes().first());
}

void ShellItemView::showInfoPopup(QModelIndex* index)
{
    if (ShellCompletionItem* item = static_cast<ShellCompletionItem*>(index->internalPointer())) {
        m_infoLabel->setText(item->description());
        m_infoPopup->show();
    }
}