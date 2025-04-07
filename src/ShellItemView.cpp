#include "ShellItemView.h"
#include "ShellCompletionItem.h"
#include "ShellCompletionModel.h"

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
    m_infoLabel = new QLabel();
    m_infoLabel->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    m_infoLabel->show();

    //setModel(new ShellCompletionModel());
    //connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &ShellItemView::onSelectionChange);
}

void ShellItemView::mouseMoveEvent(QMouseEvent* event)
{
    QModelIndex index { indexAt(viewport()->mapFrom(this, event->pos())) };
    selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    QListView::mouseMoveEvent(event);
    showInfoPopup(&index);
}

void ShellItemView::showInfoPopup(QModelIndex* index)
{
    QModelIndex descriptionIndex { model()->index(index->row(), 1, index->parent()) };
    QString desc { model()->data(descriptionIndex).toString() };
    if (!desc.isEmpty()) {
        m_infoLabel->setText(desc);
        m_infoLabel->adjustSize();
    }
}