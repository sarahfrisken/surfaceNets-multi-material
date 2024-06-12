//
// materialTable.h
//
// A GUI table to map each tissue to a group (represented as a color)

#ifndef MATERIAL_TABLE_H
#define MATERIAL_TABLE_H

#include <QTableWidget>
#include <QColor>
#include <QWidget>
#include <QTableWidget>

class GLView;

class QLabel;
class QPushButton;
class QSignalMapper;

class MaterialTable : public QTableWidget
{
	Q_OBJECT

public:
	MaterialTable(QWidget* parent = 0);

	void clear();

	void addMaterial(int label, QColor color, bool isVisible);

	void setColor(int row, QColor color);
	QColor color(int row);

	void setVisibility(int row, bool isVisible);
	bool visibility(int row);
	void toggleVisibility(int row);

	void editTableCell(int row, int col);

signals:
	void colorChanged(int row);
	void visibilityChanged(int row);

private:
	std::vector<QColor> m_colors;
	std::vector<bool> m_visibilities;

	QIcon m_visibleIcon;
	QIcon m_notVisibleIcon;

	int getRow(int label);
};

#endif