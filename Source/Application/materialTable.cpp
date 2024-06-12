//
// materialTable.cpp
//
// A GUI table to set rendering parameters for each material

#include "materialTable.h"
#include <QString>
#include <QHeaderView>
#include <QColorDialog>

MaterialTable::MaterialTable(QWidget* parent)
{
	m_visibleIcon.addPixmap(QPixmap(QString::fromUtf8("Resources/visible.png")), QIcon::Normal, QIcon::Off);
	m_notVisibleIcon.addPixmap(QPixmap(QString::fromUtf8("Resources/notVisible.png")), QIcon::Normal, QIcon::Off);

	this->setRowCount(0);
	this->setColumnCount(3);
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->horizontalHeader()->hide();
	this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	this->verticalHeader()->hide();
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->setSelectionBehavior(QAbstractItemView::SelectItems);
	this->setSelectionMode(QAbstractItemView::NoSelection);
}

void MaterialTable::clear()
{
	m_colors.clear();
	m_visibilities.clear();
	setRowCount(0);
}

void MaterialTable::addMaterial(int label, QColor color, bool isVisible)
{
	// Check to see if this material has already been added
	if (getRow(label) >= 0) return;

	// Add the new row to the table
	int idxRow = this->rowCount();
	this->insertRow(idxRow);
	this->setItem(idxRow, 0, new QTableWidgetItem());
	this->setItem(idxRow, 1, new QTableWidgetItem());
	this->setItem(idxRow, 2, new QTableWidgetItem());

	// Add the material parameters to the new row
	this->item(idxRow, 0)->setData(Qt::DisplayRole, label);
	QIcon icon = isVisible ? m_visibleIcon : m_notVisibleIcon;
	this->item(idxRow, 1)->setIcon(icon);
	this->item(idxRow, 2)->setBackgroundColor(color);

	// Store the color and visibility
	m_colors.push_back(color);
	m_visibilities.push_back(isVisible);
}

int MaterialTable::getRow(int label)
{
	for (int row = 0; row < this->rowCount(); row++) {
		int index = this->item(row, 0)->data(Qt::DisplayRole).toInt();
		if (index == label) return row;
	}
	return(-1);
}
void MaterialTable::setColor(int row, QColor color)
{
	this->item(row, 2)->setBackgroundColor(color);
	m_colors[row] = color;
}
QColor MaterialTable::color(int row)
{
	return m_colors[row];
}

void MaterialTable::setVisibility(int row, bool isVisible)
{
	m_visibilities[row] = isVisible;
	QIcon icon = isVisible ? m_visibleIcon : m_notVisibleIcon;
	this->item(row, 1)->setIcon(icon);
}
bool MaterialTable::visibility(int row)
{
	return m_visibilities[row];
}
void MaterialTable::toggleVisibility(int row)
{
	bool isVisible = !m_visibilities[row];
	setVisibility(row, isVisible);
}

void MaterialTable::editTableCell(int row, int col)
{
	if (col == 1) {
		toggleVisibility(row);
		visibilityChanged(row);
	}
	else if (col == 2) {
		QColor color = QColorDialog::getColor(m_colors[row], this);
		if (color.isValid()) {
			setColor(row, color);
			colorChanged(row);
		}
	}
}