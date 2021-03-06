#include "editparts.h"
#include "ui_editparts.h"
#include "pixmaploaddelegate.h"

#include <QIcon>
#include <QFile>
#include <QDebug>
#include <QPixmap>
#include <QBuffer>
#include <QByteArray>
#include <QFileDialog>
#include <QDataWidgetMapper>

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelation>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>

EditParts::EditParts(int id, QWidget *parent) :
    id(id),
    QDialog(parent),
    ui(new Ui::EditParts)
{
    ui->setupUi(this);

    model = new QSqlTableModel(this);
    model->setTable("parts");    
    model->setSort(0, Qt::AscendingOrder);
    model->select();

    mapper = new QDataWidgetMapper(this);
    mapper->setItemDelegate(new PixmapLoadDelegate(mapper));
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setModel(model);
    mapper->addMapping(ui->editName, 1);
    mapper->addMapping(ui->editMaterial, 2);
    mapper->addMapping(ui->spinboxWeight, 3);
    mapper->addMapping(ui->imageLabel, 4);

    if(id == 0) {
        mapper->toFirst();
    } else {
        for(int i = 0; i < model->rowCount(); ++i) {
            QSqlRecord rec = model->record(i);
            if(rec.value(0).toInt() == id) {
                mapper->setCurrentIndex(i+1);
                break;
            }
        }
    }

    connect(ui->btnNext, SIGNAL(clicked()), mapper, SLOT(toNext()));
    connect(ui->btnNext, SIGNAL(clicked()), this, SLOT(inc()));
    connect(ui->btnPrev, SIGNAL(clicked()), mapper, SLOT(toPrevious()));
    connect(ui->btnPrev, SIGNAL(clicked()), this, SLOT(dec()));
    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(add()));
    connect(ui->btnDel, SIGNAL(clicked()), this, SLOT(del()));
    connect(ui->btnFirst, SIGNAL(clicked()), mapper, SLOT(toFirst()));
    connect(ui->btnLast, SIGNAL(clicked()), mapper, SLOT(toLast()));
    connect(ui->btnFirst, SIGNAL(clicked()), this, SLOT(first()));
    connect(ui->btnLast, SIGNAL(clicked()), this, SLOT(last()));
    connect(ui->btnLoadImage, SIGNAL(clicked()), this, SLOT(loadImage()));
}

EditParts::~EditParts()
{
    delete ui;
}

void EditParts::loadImage()
{
    QString filter = "*.png, *.jpg";
    QString imgName = QFileDialog::getOpenFileName(this, tr("Загрузить изображение детали"),
                                                   tr("/home/work/Изображения"), filter);
    if(imgName.isEmpty())
        return;

    uploadedImage = new QPixmap(imgName);
    ui->imageLabel->setPixmap(*uploadedImage);
}

void EditParts::add()
{
    int row = mapper->currentIndex();
    mapper->submit();
    model->insertRow(row);
    mapper->setCurrentIndex(row);
    ui->editName->clear();
    ui->editMaterial->clear();
    ui->imageLabel->setText(tr("Нет изображения"));
    ui->spinboxWeight->setValue(0);
    ui->editName->setFocus();
    id = mapper->currentIndex();    
}

void EditParts::del()
{
    int row = mapper->currentIndex();
    model->removeRow(row);
    mapper->submit();
    mapper->setCurrentIndex(qMin(row, model->rowCount()-1));
}

void EditParts::inc()
{
    id = qMin(model->rowCount()-1, id+1);
    mapper->setCurrentIndex(id);
}

void EditParts::dec()
{
    id = qMax(0, id-1);
    mapper->setCurrentIndex(id);
}

void EditParts::accept()
{
    mapper->submit();
}

void EditParts::reject()
{
    mapper->revert();
    QDialog::reject();
}

void EditParts::last()
{
    id = model->rowCount() - 1;
}
