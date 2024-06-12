//
// setValueSliderGroup.h
//
// A GUI widget for setting a value that includes a label and a horizontal slider and
// spin box that can be used to set and display the value

#ifndef SET_VALUE_GROUP_H
#define SET_VALUE_GROUP_H

#include "qwidget.h"

#include <QSlider>
#include <QDoubleSpinBox>

class SetValueGroup : public QWidget
{
	Q_OBJECT

public:
	SetValueGroup(const QString title, float minValue, float maxValue, float value,
		QWidget *parent = 0);

	void setValue(float value);

signals:
	void valueChanged(float value);

private:
	QSlider m_slider;
	QDoubleSpinBox m_spinBox;

	float m_value;
	float m_minValue;
	float m_maxValue;
	float m_sliderNumSamples;
	float m_sliderScaleFactor;

	int valueToSliderPosition(float value);
	float sliderPositionToValue(int sliderPosition);
	
	private slots: 
	void onSliderMoved(int sliderPosition);
	void onSliderReleased();
	void onSpinBoxValueChanged(double value);
};

#endif 