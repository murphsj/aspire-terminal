#include "CompletionTree.h"

CompletionTree::CompletionTree()
    : m_treeView()
    , m_completionItems()
    , m_sortModel()
{
    m_treeView->setModel(m_completionItems);
}
/*
CompletionTree::generateCompletions()
{

}*/