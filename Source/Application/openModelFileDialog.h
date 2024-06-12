//
// newModelDialog.h
//
// Dialog for specifying parameters for generating a new model
//

#ifndef OPEN_MODEL_FILE_DIALOG_H
#define OPEN_MODEL_FILE_DIALOG_H

#include <QDialog>

class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QFormLayout;
class QComboBox;
class QDialogButtonBox;
class QSpinBox;
class QDoubleSpinBox;

class OpenModelFileDialog : public QDialog
{
	Q_OBJECT

public:
	OpenModelFileDialog(QWidget* parent = 0);

	QString getFilename();
	int getBytesPerVoxel();
	void getDimensions(int dims[3]);
	void getVoxelSize(float size[3]);

private slots:
	void onBrowseButton();

private:
	// It is important to define these on the heap, or they will be deleted after construction
	// and before the dialog is shown/executed.
	QVBoxLayout* m_dialogLayout;
	QLabel* m_title;
	QGroupBox* m_filenameGroup;
	QHBoxLayout* m_filenameLayout;
	QLabel* m_filenameLabel;
	QLineEdit* m_filenameLineEdit;
	QPushButton* m_filenameBrowseButton;
	QGroupBox* m_dataTypeGroup;
	QGroupBox* m_dimensionsGroup;
	QGroupBox* m_sizeGroup;
	QFormLayout* m_dataTypeLayout;
	QFormLayout* m_dimensionsLayout;
	QFormLayout* m_sizeLayout;
	QComboBox* m_dataType;
	QSpinBox* m_xDimInput;
	QSpinBox* m_yDimInput;
	QSpinBox* m_zDimInput;
	QDoubleSpinBox* m_xSizeInput;
	QDoubleSpinBox* m_ySizeInput;
	QDoubleSpinBox* m_zSizeInput;
	QDialogButtonBox* m_buttonBox;

	QString m_filename;
};

#endif