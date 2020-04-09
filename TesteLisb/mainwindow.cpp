#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTime time(QTime::currentTime());

    key = time.hour()*3600;
    key += time.minute()*60;
    key += time.second();

    ui->SerialBox->clear();

    ui->Pause->setEnabled(false);

    ui->RangeDefNumber->setText(QString::number(Range));

    ui->AmoDefNumber->setText(QString::number(Amostras));


    graph(ui->widget);
}

void MainWindow::graph(QCustomPlot *widget)
{
    widget->addGraph();
    widget->graph(0)->setPen(QPen(QColor(0,0,255,50))); // line color blue for first graph
    widget->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    widget->graph(0)->setLineStyle(QCPGraph::lsLine);
    widget->yAxis->setRange(0, 100);

    QSharedPointer<QCPAxisTickerTime> timeTicker (new QCPAxisTickerTime);
    timeTicker->setTimeFormat(("%h:%m:%s"));
    widget->xAxis->setTicker(timeTicker);

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(36);
}

void MainWindow::realtimeDataSlot()
{
    if(QSerialPortInfo::availablePorts().count()!=ui->SerialBox->count())
    {
        ui->SerialBox->clear();
        for (QSerialPortInfo &info : QSerialPortInfo::availablePorts()) ui->SerialBox->addItem(info.portName());
    }

    if (!pause){
        key += 0.036;
        QString pontos[2] = {"000000000","000000000"}, estabilidade;
        double defAux = 0, pico = 0;

        if (!flag){
            if(serial.isOpen()) {
                serial.clear();
                flag = true;
            }
        }

        if(serial.isOpen()) {
            QByteArray readData = serial.readAll();
            for (int i=0; i<readData.length(); i++){ //para a certeza de pegar os valores corretos
                if (int(readData[i]) == 55)
                    if (int(readData[i+1]) == 45){
                        for ( int i=0; i<8; i++) passa[i]= false;
                        passa2[0] = false;
                        passa2[1] = false;
                        pontos[0] = QString::number(readData[i+2],2);
                        pontos[1] = QString::number(readData[i+3],2);
                        defAux = (double(readData[i+4])+double(readData[i+5])/100); //mudar aqui caso mudar no codeC
                        if (defAux>defasagem+TDefasagem || defAux<defasagem-TDefasagem) defasagem = floorf(defAux*100)/100; //+ e - dentro do if = range para alteração do valor
                        if (defasagem != 0){
                            pico = double(readData[i+6])*40+double(readData[i+7])*40/100;
                            if (pico>2100) estabilidade = "estável";
                            else
                                estabilidade = "instável";
                        }
                        else {
                            pico = 0;
                            estabilidade = "indefinida";
                        }
                        cont = 0;
                        for (int i=0; i<pontos[0].length(); i++){
                            if (pontos[0][i] == '1')
                                cont ++;
                        }
                        for (int i=0; i<pontos[1].length(); i++){
                            if (pontos[1][i] == '1')
                                cont ++;
                        }
                        current = double(cont)*100/9;
                        ui->statusBar->showMessage(" Horário = "
                        + QString::number(floorf(key/3600)).rightJustified(2, '0') + ":" + QString::number(floorf(key/60-floorf(key/3600)*60)).rightJustified(2, '0') + ":"
                        + QString::number(floorf((key-floorf(key/60-floorf(key/3600)*60)*60-floorf(key/3600)*3600))).rightJustified(2, '0') + ":"
                        + QString::number(floorf(((key-floorf(key/60-floorf(key/3600)*60)*60-floorf(key/3600)*3600)-floorf((key-floorf(key/60-floorf(key/3600)*60)*60-floorf(key/3600)*3600)))*100)).rightJustified(2, '0')
                        + "  |  Valor percentual de água presente = " + QString::number(floorf(current)));// + "%  |  Estabilidade = " + estabilidade + "  |  Defasagem (em microseg) = "
                       // + QString("%1").arg(defasagem, 0, 'f', 2)  + "  |  Pontos ativos =  " + pontos[1] + " " + pontos[0]);
                    }
            }
            serial.clear();
        }
        else{
            ui->statusBar->showMessage(" Sem conexão");
            current = 0;
            ui->NAmostras->setText("0");
        }

        if (serial.isOpen()) for (int i=pontos[0].length(); i>0; i--) if (pontos[0][pontos[0].length()-i]=='1') passa[i-1] = true;
        if (serial.isOpen()) for (int i=pontos[1].length(); i>0; i--) if (pontos[1][pontos[1].length()-i]=='1') passa2[i-1] = true;

        QPixmap green = QPixmap(":/pics/greenAlert.png");
        green = green.scaled(21, 21, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap red = QPixmap(":/pics/redAlert.png");
        red = red.scaled(21, 21, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if (passa[0]) ui->alert1->setPixmap(green);
        else ui->alert1->setPixmap(red);
        if (passa[1]) ui->alert2->setPixmap(green);
        else ui->alert2->setPixmap(red);
        if (passa[2]) ui->alert3->setPixmap(green);
        else ui->alert3->setPixmap(red);
        if (passa[3]) ui->alert4->setPixmap(green);
        else ui->alert4->setPixmap(red);
        if (passa[4]) ui->alert5->setPixmap(green);
        else ui->alert5->setPixmap(red);
        if (passa[5]) ui->alert6->setPixmap(green);
        else ui->alert6->setPixmap(red);
        if (passa[6]) ui->alert7->setPixmap(green);
        else ui->alert7->setPixmap(red);
        if (passa2[0]) ui->alert8->setPixmap(green);
        else ui->alert8->setPixmap(red);
        if (passa2[1]) ui->alert9->setPixmap(green);
        else ui->alert9->setPixmap(red);

        contaexec++;
        media += current;
        if ((contaexec==(((Range*10)+1)-Amostras)) || (resetgraph)){
            //if (media!=mediaanterior) armazenaMudanca();
            if(serial.isOpen()) ui->NAmostras->setText(QString::number(((Range*10)+1)-Amostras));
            media = media/(contaexec);
            ui->widget->xAxis->setRange(key-Range, Range, Qt::AlignLeft);
            ui->widget->graph(0)->addData(key, media);
            ui->widget->replot();
            contaexec = 0;
            media = 0;
            resetgraph = false;
        }
        mediaanterior = media;
    }

}

void armazenaMudanca(){
    //adiciona uma linha num arquivo SQL ou EXCEL contendo o horário e a nova porcentagem
}

MainWindow::~MainWindow()
{
    if (serial.isOpen()) serial.close();
    delete ui;
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    if (checked){
        serial.setPortName(port_name);
        serial.setBaudRate(115200);
        serial.open(QIODevice::ReadOnly);
        if (serial.isOpen()) ui->Pause->setEnabled(true);
    }
    else{
        if (serial.isOpen()) serial.close();
        flag = false;
        for ( int i=0; i<8; i++) passa[i]= false;
        passa2[0] = false;
        passa2[1] = false;
        ui->Pause->setEnabled(false);
    }
}

void MainWindow::on_SerialBox_currentIndexChanged(const QString &arg1)
{
    port_name=arg1;
}

void MainWindow::on_Pause_clicked(bool checked)
{
    if (!pause){
        pause = true;
        ui->Pause->setText("Continuar");

        ui->widget->xAxis->setRange(key-2+0.009, 2+0.009, Qt::AlignLeft);
        ui->widget->graph(0)->addData(key+0.009, 0);
    }
    else{
        QTime time(QTime::currentTime());

        key = time.hour()*3600;
        key += time.minute()*60;
        key += time.second();

        ui->widget->xAxis->setRange(key-2-0.009, 2-0.009, Qt::AlignLeft);
        ui->widget->graph(0)->addData(key-0.009, 0);

        pause = false;
        ui->Pause->setText("Pausar");
    }
}

void MainWindow::on_RangeDefSlider_valueChanged(int value)
{
    Range = value;
    ui->RangeDefNumber->setText(QString::number(value));
    resetgraph = true;
}


void MainWindow::on_RangeDefNumber_textChanged(const QString &arg1)
{
    Rangeaux = QString(arg1).toInt();
}



void MainWindow::on_RangeDefNumber_returnPressed()
{
    if (Rangeaux>=2 && Rangeaux<=10){
        Range = Rangeaux;
        ui->RangeDefSlider->setSliderPosition(Rangeaux);
    }
}

void MainWindow::on_RangeDefNumber_selectionChanged()
{
    ui->RangeDefNumber->setText(QString::number(Range));
}

void MainWindow::on_AmoDefSlider_valueChanged(int value)
{
    Amostras = value;
    ui->AmoDefNumber->setText(QString::number(value));
    resetgraph = true;
}

void MainWindow::on_AmoDefNumber_textChanged(const QString &arg1)
{
    Amostrasaux = QString(arg1).toInt();
}

void MainWindow::on_AmoDefNumber_returnPressed()
{
    if (Amostrasaux>=5 && Amostrasaux<=20){
        Amostras = Amostrasaux;
        ui->AmoDefSlider->setSliderPosition(Amostrasaux);
    }
}

void MainWindow::on_MainWindow_iconSizeChanged(const QSize &iconSize)
{
    //ui->sensor->setGeometry(iconSize.width()-318, this, this, this);
}
