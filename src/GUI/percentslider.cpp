#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QLocale>
#include "units.h"
#include "percentslider.h"


static QString format(int value)
{
	return QLocale::system().toString(value) + UNIT_SPACE + QString("%");
}

PercentSlider::PercentSlider(QWidget *parent) : QWidget(parent)
{
	_slider = new QSlider(Qt::Horizontal);
	_label = new QLabel();

	_slider->setMinimum(0);
	_slider->setMaximum(100);

	QFontMetrics fm(_label->font());
	_label->setFixedWidth(fm.boundingRect(format(_slider->maximum())).width());
	_label->setAlignment(Qt::AlignRight);
	_label->setText(format(_slider->value()));

	connect(_slider, &QSlider::valueChanged, this, &PercentSlider::updateLabel);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(_slider);
	layout->addWidget(_label);

	setLayout(layout);
}

void PercentSlider::updateLabel(int value)
{
	_label->setText(format(value));
}

int PercentSlider::value() const
{
	return _slider->value();
}

void PercentSlider::setValue(int value)
{
	_slider->setValue(value);
	_label->setText(format(value));
}
