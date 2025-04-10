#include "ShellItemView.h"

#include <QListView>
#include <QItemSelectionModel>
#include <QScrollBar>
#include <qnamespace.h>
#include <QObject>

ShellItemView::ShellItemView():
    QListView()
{
    init();
}

void ShellItemView::init()
{
    setMouseTracking(true);
    m_infoLabel = new QLabel(this);
    m_infoLabel->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_infoLabel->setAttribute(Qt::WA_ShowWithoutActivating);
    m_infoLabel->setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFocusPolicy(Qt::NoFocus);
    m_infoLabel->raise();
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
        QPoint itemLocation { mapToGlobal(visualRect(*index).topLeft()) };
        m_infoLabel->move(itemLocation.x() + size().width(), itemLocation.y());
        m_infoLabel->setText(desc);
        m_infoLabel->adjustSize();
        m_infoLabel->show();
    }
}

void ShellItemView::hideEvent(QHideEvent* event)
{
    m_infoLabel->hide();
    QListView::hideEvent(event);
}

void ShellItemView::showEvent(QShowEvent* event)
{
    m_infoLabel->show();
    QListView::showEvent(event);
}