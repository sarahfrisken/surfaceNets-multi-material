//
// SetValueSliderGroup.cpp
//
// A GUI widget for setting a value that includes a label and a horizontal slider and
// spin box that can be used to set and display the value

#include "setValueGroup.h"

#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QSignalMapper>

SetValueGroup::SetValueGroup(const QString title, float minValue, float maxValue, float value,
	QWidget* parent)
	:
	m_sliderNumSamples(100),
	m_minValue(minValue),
	m_maxValue(maxValue), 
	m_sliderScaleFactor(0),
	m_value(value)
{
	if (m_minValue >= m_maxValue) return;
	m_sliderScaleFactor = ((float)(m_maxValue - m_minValue)) / ((float)m_sliderNumSamples);
	m_slider.setOrientation(Qt::Horizontal);
	m_slider.setRange(0, m_sliderNumSamples);
	m_slider.setTickPosition(QSlider::TicksBelow);
	m_slider.setTickInterval(10);
	m_slider.setSingleStep(1);

	m_spinBox.setRange(minValue, maxValue);
	m_spinBox.setSingleStep(m_sliderScaleFactor);

	QGroupBox *groupBox = new QGroupBox(title);
	QHBoxLayout *hLayout = new QHBoxLayout(parent);
	hLayout->addWidget(&m_slider);
	hLayout->addWidget(&m_spinBox);
	groupBox->setLayout(hLayout);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(groupBox);
	this->setLayout(layout);

	connect(&m_slider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMoved(int)));
	connect(&m_slider, SIGNAL(sliderReleased()), this, SLOT(onSliderReleased()));
	connect(&m_spinBox, SIGNAL(valueChanged(double)), this, SLOT(onSpinBoxValueChanged(double)));

	setValue(value);
}

void SetValueGroup::setValue(float value)
{
	if (value < m_minValue) value = m_minValue;
	if (value > m_maxValue) value = m_maxValue;
	m_value = value;
	m_spinBox.setValue(m_value);
}

int SetValueGroup::valueToSliderPosition(float value)
{
	return (int)((value - m_minValue) / m_sliderScaleFactor);
}
float SetValueGroup::sliderPositionToValue(int sliderPosition)
{
	return ((float)sliderPosition * m_sliderScaleFactor + m_minValue);
}

void SetValueGroup::onSpinBoxValueChanged(double value)
{
	m_value = value;
	m_slider.setValue(valueToSliderPosition(m_value));

	// Spin box value is changed when it is manipulated, the slider is released or
	// the slider moves. A signal is only sent when the slider is not being moved.
	if (!m_slider.isSliderDown()) {
		valueChanged(value);
	}
}
void SetValueGroup::onSliderMoved(int sliderPosition)
{
	m_value = sliderPositionToValue(sliderPosition);
	m_spinBox.setValue(m_value);
}
void SetValueGroup::onSliderReleased()
{
	valueChanged(m_value);
}
