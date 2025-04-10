#ifndef ASPIRE_TERMINAL_SHELL_ITEM_VIEW
#define ASPIRE_TERMINAL_SHELL_ITEM_VIEW

#include <QListView>
#include <QMouseEvent>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QWidget>
#include <QLabel>

class ShellItemView : public QListView
{
public:
    explicit ShellItemView();

private slots:
    void onSelectionChange(const QItemSelection& selected, const QItemSelection& deselected);

protected:
    void hideEvent(QHideEvent*) override;
    void showEvent(QShowEvent*) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void init();
    void showInfoPopup(QModelIndex* index);

    QWidget* m_infoPopup;
    QLabel* m_infoLabel;
};

#endif