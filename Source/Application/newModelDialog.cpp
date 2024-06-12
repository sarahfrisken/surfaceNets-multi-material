//
// newModelDialog.cpp
//
// Dialog for specifying parameters for generating a new model
//

#include "newModelDialog.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

NewModelDialog::NewModelDialog(QWidget* parent) :
	m_numSpheresInput(nullptr),
	m_xDimInput(nullptr),
	m_yDimInput(nullptr),
	m_zDimInput(nullptr),
	m_xSizeInput(nullptr),
	m_ySizeInput(nullptr),
	m_zSizeInput(nullptr)
{
	setWindowTitle(tr("New"));
	m_title = new QLabel(tr("Create a new model of random spheres"));
	m_dialogLayout = new QVBoxLayout();
	setLayout(m_dialogLayout);
	m_dialogLayout->addWidget(m_title);

	// Set default values
	int numSpheres = 1;
	int arraySize[3] = { 64, 64, 64 };
	float voxelSize[3] = { 1, 1, 1 };

	// Get #spheres
	m_spheresGroup = new QGroupBox();
	m_numSpheresLayout = new QFormLayout();
	m_numSpheresInput = new QSpinBox;
	m_numSpheresInput->setValue(numSpheres);
	m_numSpheresLayout->addRow(tr("Number of spheres:"), m_numSpheresInput);
	m_spheresGroup->setLayout(m_numSpheresLayout);
	m_dialogLayout->addWidget(m_spheresGroup);

	// Get dimensions
	m_dimensionsGroup = new QGroupBox("Dimensions");
	m_dimensionsLayout = new QFormLayout();
	m_xDimInput = new QSpinBox;
	m_yDimInput = new QSpinBox;
	m_zDimInput = new QSpinBox;
	m_xDimInput->setValue(arraySize[0]);
	m_yDimInput->setValue(arraySize[1]);
	m_zDimInput->setValue(arraySize[2]);
	m_xDimInput->setMaximum(256);
	m_yDimInput->setMaximum(256);
	m_zDimInput->setMaximum(256);
	m_dimensionsLayout->addRow(tr("X dimension:"), m_xDimInput);
	m_dimensionsLayout->addRow(tr("Y dimension:"), m_yDimInput);
	m_dimensionsLayout->addRow(tr("Z dimension:"), m_zDimInput);
	m_dimensionsGroup->setLayout(m_dimensionsLayout);
	m_dialogLayout->addWidget(m_dimensionsGroup);

	// Get voxel sizes
	m_sizeGroup = new QGroupBox("Voxel size");
	m_sizeLayout = new QFormLayout;
	m_xSizeInput = new QDoubleSpinBox;
	m_ySizeInput = new QDoubleSpinBox;
	m_zSizeInput = new QDoubleSpinBox;
	m_xSizeInput->setValue(voxelSize[0]);
	m_ySizeInput->setValue(voxelSize[1]);
	m_zSizeInput->setValue(voxelSize[2]);
	m_sizeLayout->addRow(tr("X dimension:"), m_xSizeInput);
	m_sizeLayout->addRow(tr("Y dimension:"), m_ySizeInput);
	m_sizeLayout->addRow(tr("Z dimension:"), m_zSizeInput);
	m_sizeGroup->setLayout(m_sizeLayout);
	m_dialogLayout->addWidget(m_sizeGroup);

	// Cancel/OK buttons
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	m_dialogLayout->addWidget(m_buttonBox);
	connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

int NewModelDialog::getNumSpheres()
{
	return(m_numSpheresInput->value());
};
void NewModelDialog::getDimensions(int dims[3])
{
	dims[0] = m_xDimInput->value();
	dims[1] = m_yDimInput->value();
	dims[2] = m_zDimInput->value();
};
void NewModelDialog::getVoxelSize(float size[3])
{
	size[0] = m_xSizeInput->value();
	size[1] = m_ySizeInput->value();
	size[2] = m_zSizeInput->value();
};