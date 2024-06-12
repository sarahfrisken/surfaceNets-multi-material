//
// newModelDialog.h
//
// Dialog for specifying parameters for generating a new model
//

#ifndef NEW_MODEL_DIALOG_H
#define NEW_MODEL_DIALOG_H

#include <QDialog>

class QVBoxLayout;
class QGroupBox;
class QLabel;
class QFormLayout;
class QDialogButtonBox;
class QSpinBox;
class QDoubleSpinBox;

class NewModelDialog : public QDialog
{
	Q_OBJECT

public:
	NewModelDialog(QWidget* parent = 0);

	int getNumSpheres();
	void getDimensions(int dims[3]);
	void getVoxelSize(float size[3]);

private:
	// Its important to define these on the heap, or they will be deleted after construction
	// and before the dialog is shown/executed.
	QVBoxLayout* m_dialogLayout;
	QLabel* m_title;
	QGroupBox* m_spheresGroup;
	QGroupBox* m_dimensionsGroup;
	QGroupBox* m_sizeGroup;
	QFormLayout* m_numSpheresLayout;
	QFormLayout* m_dimensionsLayout;
	QFormLayout* m_sizeLayout;
	QSpinBox* m_numSpheresInput;
	QSpinBox* m_xDimInput;
	QSpinBox* m_yDimInput;
	QSpinBox* m_zDimInput;
	QDoubleSpinBox* m_xSizeInput;
	QDoubleSpinBox* m_ySizeInput;
	QDoubleSpinBox* m_zSizeInput;
	QDialogButtonBox* m_buttonBox;
};

#endif