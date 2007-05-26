//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 200-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include <QtGui>

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QFile data("../src/gptexturepalette.cpp");

	if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
	out << "const uint TextureColorizer::texturepalette[][512]=" << endl
	    << "{" << endl;
	out.setIntegerBase(16);

	QImage* gradimg = new QImage(256,10,QImage::Format_RGB32);

	QStringList filelist;
	filelist << "../data/seacolors.leg" << "../data/landcolors.leg";
	QString filename;

	QPainter painter(gradimg);
	painter.setPen(Qt::NoPen);

	for (int j=0; j < 16; j++){
		foreach(filename, filelist){
		
			QLinearGradient gradient(0, 0, 256, 0);

			QFile file( filename );
			file.open( QIODevice::ReadOnly );
    			QTextStream stream( &file );  // read the data serialized from the file
       		 	QString evalstrg;

		        while(!stream.atEnd()){
       			     stream >> evalstrg;
				if (!evalstrg.isEmpty() && evalstrg.contains("=")){
					QString colval = evalstrg.section("=",0,0);
					QString colpos = evalstrg.section("=",1,1);
					gradient.setColorAt(colpos.toDouble(), QColor(colval));
				}
			}
			painter.setBrush(gradient);
			painter.drawRect(0, 0, 256, 10);	

			int alpha = 2*j;

			for (int i = 0; i < 256; i++){
				if (filename==filelist[0]){
					QRgb palcol = gradimg->pixel(i,1);
					int red = (qRed(palcol)-alpha) > 0 ? qRed(palcol)-alpha : 0;
					int green = (qGreen(palcol)-alpha) > 0 ? qGreen(palcol)-alpha : 0;
					int blue = (qBlue(palcol)-alpha) > 0 ? qBlue(palcol)-alpha : 0;
					palcol=qRgb(red,green,blue);

					if ( i == 0 )
						out << "		{ 0x" << (uint)palcol << "," << endl;
					else
						out << "		0x" << (uint)palcol << "," << endl;
				}
				else {
					QRgb palcol = gradimg->pixel(i,1);
					int red = (qRed(palcol)-alpha) > 0 ? qRed(palcol)-alpha : 0;
					int green = (qGreen(palcol)-alpha) > 0 ? qGreen(palcol)-alpha : 0;
					int blue = (qBlue(palcol)-alpha) > 0 ? qBlue(palcol)-alpha : 0;
					palcol=qRgb(red,green,blue);

					if ( i == 255 ) 
						out << "		0x" << (uint)palcol << " }," << endl;
					else
						out << "		0x" << (uint)palcol << "," << endl;
//					out << "		" << (uint)gradimg->pixel(i,1) << "," << endl;
				}
			}
		}
	}

        out << "};" << endl;

//<< qSetFieldWidth(10) << left << 3.14 << 2.7 << endl;
        // writes "Result: 3.14      2.7       \n"
    }

        app.exit();
}
