#include "file_import_dialog.hpp"
#include "map_overlay_collection.hpp"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QFileInfo>
#include <QDebug>
#include <QTimer>


FileImportDialog::FileImportDialog(const QStringList &files, quint64 parent_folder_id, QSharedPointer<MapOverlayManager> index, QWidget *parent) : QDialog(parent)
{
    progress_bar_ = new QProgressBar(this);
    name_label_ = new QLabel ("Name");

    stop_button_ = new QPushButton("&Stop");

    QHBoxLayout *button_layout = new QHBoxLayout;
    button_layout->addStretch(1);
    button_layout->addWidget(stop_button_);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(name_label_);
    vbox->addWidget(progress_bar_);
    vbox->addLayout(button_layout);
    setLayout(vbox);

    setMinimumWidth(333);

    progress_bar_->setRange(0, files.size() - 1) ;

    worker_ = new FileImportWorker(files, parent_folder_id, index);

    connect(worker_, SIGNAL(finished()), this, SLOT(finished()));
    connect(worker_, SIGNAL(importStarted(QString)), this, SLOT(setMessage(QString)));
    connect(worker_, SIGNAL(importFinished(int)), this, SLOT(setProgress(int)));
    connect(stop_button_, SIGNAL(clicked()), this, SLOT(stop()));
    connect(worker_, SIGNAL(terminated()), this, SLOT(close()));

    QTimer::singleShot( 0, this, SLOT(start()) );

}

void FileImportDialog::start() {
    worker_->start();
}

void FileImportDialog::setMessage(const QString &txt)
{
    name_label_->setText(txt) ;
}

void FileImportDialog::setProgress(int pos)
{

    progress_bar_->setValue(pos);
}

void FileImportDialog::stop()
{
    worker_->stop_flag_ = 1 ;
    close() ;
}

void FileImportDialog::finished()
{
    collections_ = worker_->collections_ ;
    documents_ = worker_->documents_ ;
    worker_->deleteLater();

    close() ;
}

void FileImportWorker::run()
{
    for(int i=0 ; i<files_.size() ; i++)
    {
        if ( stop_flag_ ) break ;
        const QString &file_name = files_[i] ;
        emit importStarted(QFileInfo(file_name).fileName()) ;
/*
        if ( QFileInfo(file_name).completeSuffix().toUpper() == "GPX" )  {
            CollectionData *col = importGpx(file_name, parent_folder_id_, overlay_manager_) ;

            collections_.append(col) ;
        }
        else if ( QFileInfo(file_name).completeSuffix().toUpper() == "KMZ"  )
        {
            CollectionTreeNode *cl =  importKmz(file_name, parent_folder_id_, overlay_manager_);

            if ( cl ) documents_.append(cl) ;
        }
        else if ( QFileInfo(file_name).completeSuffix().toUpper() == "KML"  )
        {
            CollectionTreeNode *cl =  importKml(file_name, parent_folder_id_, overlay_manager_);

            if ( cl ) documents_.append(cl) ;
        }
*/
        emit importFinished(i) ;
    }
}
