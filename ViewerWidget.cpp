#include "ViewerWidget.h"
#include <vector>
#include <algorithm>
#include <random>

ViewerWidget::ViewerWidget(QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

bool ViewerWidget::changeSize(int width, int height)
{
	QSize newSize(width, height);

	if (newSize != QSize(0, 0)) {
		if (img != nullptr) {
			delete img;
		}

		img = new QImage(newSize, QImage::Format_ARGB32);
		if (!img) {
			return false;
		}
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		update();
	}

	return true;
}

//Draw functions
//void ViewerWidget::freeDraw(QPoint end, QPen pen)
//{
//	painter->setPen(pen);
//	//painter->drawLine(freeDrawBegin, end);
//
//	update();
//}

void ViewerWidget::clear()
{
	img->fill(Qt::white);
	update();
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}

void ViewerWidget::drawAxes(double minX, double maxX, double minY, double maxY)
{
	if (!img) return;
	img->fill(Qt::white);
	if (!painter) setPainter();

	const int marginL = 40, marginR = 20, marginTop = 20, marginBottom = 40;
	const int Wpx = img->width();
	const int Hpx = img->height();
	const int plotW = Wpx - marginL - marginR;
	const int plotH = Hpx - marginTop - marginBottom;

	painter->setPen(QPen(Qt::black, 1));
	// Y axis
	painter->drawLine(marginL, marginTop, marginL, marginTop + plotH);
	// X axis
	double zeroY = marginTop + plotH - (0 - minY) / (maxY - minY) * plotH;
	if (zeroY < marginTop) zeroY = marginTop;
	if (zeroY > marginTop + plotH) zeroY = marginTop + plotH;
	painter->drawLine(marginL, zeroY, marginL + plotW, zeroY);

	// Ticks and labels (simplified)
	painter->drawText(marginL - 35, marginTop + 5, QString::number(maxY, 'f', 1));
	painter->drawText(marginL - 35, marginTop + plotH, QString::number(minY, 'f', 1));
	painter->drawText(marginL, marginTop + plotH + 15, QString::number(minX, 'f', 1));
	painter->drawText(marginL + plotW - 20, marginTop + plotH + 15, QString::number(maxX, 'f', 1));

	update();
}

void ViewerWidget::drawTrajectories(const std::vector<std::vector<double>>& trajectories, double T)
{
	if (trajectories.empty()) return;

	double minY = trajectories[0][0];
	double maxY = trajectories[0][0];

	for (const auto& traj : trajectories) {
		for (double val : traj) {
			if (val < minY) minY = val;
			if (val > maxY) maxY = val;
		}
	}

	// Add some padding to Y axis
	double padding = (maxY - minY) * 0.1;
	if (padding == 0) padding = 1.0;
	minY -= padding;
	maxY += padding;

	drawAxes(0, T, minY, maxY);

	const int marginL = 40, marginR = 20, marginTop = 20, marginBottom = 40;
	const int Wpx = img->width();
	const int Hpx = img->height();
	const int plotW = Wpx - marginL - marginR;
	const int plotH = Hpx - marginTop - marginBottom;

	std::mt19937 colorGen(272);
	std::uniform_int_distribution<int> colorDist(0, 200);

	for (const auto& traj : trajectories) {
		if (traj.size() < 2) continue;

		painter->setPen(QPen(QColor(colorDist(colorGen), colorDist(colorGen), colorDist(colorGen), 150), 1));

		int N = traj.size() - 1;
		for (int i = 0; i < N; i++) {
			double x1 = marginL + (double)i / N * plotW;
			double y1 = marginTop + plotH - (traj[i] - minY) / (maxY - minY) * plotH;
			double x2 = marginL + (double)(i + 1) / N * plotW;
			double y2 = marginTop + plotH - (traj[i + 1] - minY) / (maxY - minY) * plotH;
			painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
		}
	}

	update();
}