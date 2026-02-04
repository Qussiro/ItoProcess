#include "ImageViewer.h"
#include <cmath>
#include <random>
#include <vector>
#include <QDebug>

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	vW = new ViewerWidget(QSize(500, 500));
	ui->scrollArea->setWidget(vW);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(true);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
	/*QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->button() == Qt::LeftButton) {
		w->setFreeDrawBegin(e->pos());
		w->setFreeDrawActivated(true);
	}*/
}
void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	/*QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->button() == Qt::LeftButton && w->getFreeDrawActivated()) {
		w->freeDraw(e->pos(), QPen(Qt::red));
		w->setFreeDrawActivated(false);
	}*/
}
void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	/*QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->buttons() == Qt::LeftButton && w->getFreeDrawActivated()) {
		w->freeDraw(e->pos(), QPen(Qt::red));
		w->setFreeDrawBegin(e->pos());
	}*/
}
void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}
void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
bool ImageViewer::openImage(QString filename)
{
	QImage loadedImg(filename);
	if (!loadedImg.isNull()) {
		return vW->setImage(loadedImg);
	}
	return false;
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();

	QImage* img = vW->getImage();
	return img->save(filename, extension.toStdString().c_str());
}

//Slots
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	QString folder = settings.value("folder_img_save_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder, fileFilter);
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

		if (!saveImage(fileName)) {
			msgBox.setText("Unable to save image.");
			msgBox.setIcon(QMessageBox::Warning);
		}
		else {
			msgBox.setText(QString("File %1 saved.").arg(fileName));
			msgBox.setIcon(QMessageBox::Information);
		}
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered()
{
	vW->clear();
}
void ImageViewer::on_actionExit_triggered()
{
	this->close();
}

void ImageViewer::on_pushButtonGenerate_clicked()
{
	int numTraj = ui->spinBoxCountTrajectories->value();
	double T = ui->spinBoxTime->value();
	int N = ui->spinBoxSteps->value();
	double omega = ui->spinBoxAngle->value();
	double dt = T / N;
	
	int processType = ui->comboBoxItoovProces->currentIndex();

	std::vector<std::vector<double>> trajectories(numTraj);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<double> norm_dist(0.0, 1.0);

	for (int i = 0; i < numTraj; i++) {
		trajectories[i].resize(N + 1);
		double currentW = 0.0;

		trajectories[i][0] = (processType == 2) ? 1.0 : 0.0;

		for (int j = 1; j <= N; j++) {
			double t = j * dt;
			currentW += std::sqrt(dt) * norm_dist(gen);

			switch (processType) {
			case 0: // Wiener process
				trajectories[i][j] = currentW;
				break;
			case 1: // Task 18.3.2: X_t = W_t * cos(omega * t)
				trajectories[i][j] = currentW * std::cos(omega * t);
				break;
			case 2: // Task 18.3.8: Y_t = (1 + W_t^2) / (1 + t)
				trajectories[i][j] = (1.0 + currentW * currentW) / (1.0 + t);
				break;
			default:
				trajectories[i][j] = currentW;
				break;
			}
		}
	}

	double minY = trajectories[0][0];
	double maxY = trajectories[0][0];
	for (double val : trajectories[0]) {
		if (val < minY) minY = val;
		if (val > maxY) maxY = val;
	}
	qDebug() << ">> Trajectory: \n" << trajectories[0] << "\n";
	qDebug() << "MIN/MAX: [" << minY << "/" << maxY << "]\n";

	vW->drawTrajectories(trajectories, T);
}

//void ImageViewer::on_spinBoxCountTrajectories_valueChanged(int value)
//{
//	on_pushButtonGenerate_clicked();
//}