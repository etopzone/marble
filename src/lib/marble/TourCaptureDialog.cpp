//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014   Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "TourCaptureDialog.h"
#include "ui_TourCaptureDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

#include "MarbleWidget.h"
#include "MovieCapture.h"
#include "TourPlayback.h"

namespace Marble {

TourCaptureDialog::TourCaptureDialog(MarbleWidget *widget, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TourCaptureDialog),
    m_recorder(new MovieCapture(widget, parent)),
    m_playback(0),
    m_writingPossible( true ),
    m_current_position( 0.0 )
{
    ui->setupUi(this);
    m_recorder->setSnapshotMethod(MovieCapture::DataDriven);
    ui->progressBar->setValue(0);

    connect(ui->fpsSlider, SIGNAL(valueChanged(int)),
            ui->fpsSpin, SLOT(setValue(int)));

    connect(ui->fpsSpin, SIGNAL(valueChanged(int)),
            ui->fpsSlider, SLOT(setValue(int)));

    connect(ui->fpsSlider, SIGNAL(valueChanged(int)),
            m_recorder, SLOT(setFps(int)));

    connect(ui->closeButton, SIGNAL(clicked()),
            this, SLOT(close()));

    connect(ui->startButton, SIGNAL(clicked()),
            this, SLOT(startRecording()));

    connect(ui->startButton, SIGNAL(clicked()),
            this, SLOT(disableStopButton()));

    connect(ui->openButton, SIGNAL(clicked()),
            this, SLOT(loadDestinationFile()));

    connect(m_recorder, SIGNAL(rateCalculated(double)),
            this, SLOT(setRate(double))) ;

    connect(m_recorder, SIGNAL(errorOccured()),
            this, SLOT(handleError()) );
}

TourCaptureDialog::~TourCaptureDialog()
{
    delete ui;
}

void TourCaptureDialog::loadDestinationFile()
{
    const QString destination =
            QFileDialog::getSaveFileName(this, tr("Save video file"), "",
                                         tr("Supported video (*.mp4 *.webm *.ogg)"));

    if (destination.isEmpty()) {
        return;
    }

    QStringList formats;
    formats << "mp4" << "webm" << "ogg";

    bool supported = false;
    foreach(const QString &format, formats) {
        if (destination.endsWith('.'+format))
            supported = true;
    }

    if (!supported) {
        QMessageBox::warning(this, tr("Filename is not valid"),
                             tr("This file format is not supported. "
                                "Please, use .ogg, .mp4 or .webm instead"));
        return;
    }

    ui->destinationEdit->setText(destination);
    m_recorder->setFilename(destination);
}

void TourCaptureDialog::startRecording()
{
    if( ui->startButton->text() == tr("Start") ){
        ui->startButton->setText(tr("Cancel"));
        const QString path = ui->destinationEdit->text();

        if( path.isEmpty() ){
            QMessageBox::warning(this, tr("Missing filename"),
                           tr("Destination video file is not set. "
                              "I don't know where to save recorded "
                              "video. Please, specify one."));
            return;
        }

        m_recorder->setSnapshotMethod( MovieCapture::DataDriven );
        m_recorder->setFps(ui->fpsSlider->value());
        m_recorder->startRecording();
        m_current_position = 0.0;
        recordNextFrame();
    }
    else{
        ui->startButton->setText(tr("Start"));
        m_recorder->cancelRecording();
        ui->progressBar->setValue(0);
        ui->closeButton->setEnabled(true);
    }
}

void TourCaptureDialog::updateProgress(double position)
{
    ui->progressBar->setValue(position);
}

void TourCaptureDialog::recordNextFrame()
{
    double const shift = 1.0 / ( ui->fpsSlider->value() );
    double const duration = m_playback->duration();

    if (!m_writingPossible) {
        ui->rate->setText(QString("<font color=\"red\">%1</font>").arg(tr("Video writing failed.")));
        return;
    }

    if( ui->startButton->text() == QString("Start") ) {
        return;
    }

    if (m_current_position <= duration) {
        m_playback->seek( m_current_position );
        m_recorder->recordFrame();
        updateProgress( m_current_position * 100 );
        m_current_position += shift;
        QTimer::singleShot(0, this, SLOT(recordNextFrame()));
    } else {
        m_recorder->stopRecording();
        ui->progressBar->setValue(duration*100);
        ui->startButton->setText(tr("Start"));
        ui->rate->setText(QString("<font color=\"green\">%1</font>").arg(tr("Video export completed.")));
        ui->closeButton->setEnabled(true);
    }
}

void TourCaptureDialog::setRate(double rate)
{
    ui->rate->setText(QString("%1 KBytes/sec").arg(rate));
}

void TourCaptureDialog::stopRecording()
{
    m_recorder->stopRecording();
}

void TourCaptureDialog::setTourPlayback( TourPlayback* playback )
{
    m_playback = playback;
    ui->progressBar->setMaximum(playback->duration() * 100);
}

void TourCaptureDialog::handleError()
{
    m_writingPossible = false;
}

void TourCaptureDialog::disableStopButton()
{
    ui->closeButton->setDisabled(true);
}

} // namespace Marble

#include "TourCaptureDialog.moc"
