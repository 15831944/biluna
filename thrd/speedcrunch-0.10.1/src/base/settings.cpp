// This file is part of the SpeedCrunch project
// Copyright (C) 2004, 2005, 2007, 2008 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.


#include "settings.hxx"

#include "3rdparty/util/binreloc.h"
#include "math/floatconfig.h"

#include <QApplication>
#include <QDir>
#include <QSettings>

#include <stdlib.h>


QSettings * createQSettings( const QString & key );

// public

SC_Settings::SC_Settings()
{
  //escape = "\\"; //reftbl
}

void SC_Settings::load()
{
  const QString KEY = "SpeedCrunch";

  QSettings * settings = createQSettings( KEY );
  if ( ! settings )
    return;

  QString key;

  key = KEY + "/General/";

  // angle mode special case
  QString angleModeStr;
  angleModeStr = settings->value( key + "AngleMode", "r" ).toString();
  if ( angleModeStr != "r" && angleModeStr != "d" )
    angleMode = 'r';
  else
    angleMode = angleModeStr[0].toLatin1();

  // radix character special case
  QString radixCharStr;
  radixCharStr = settings->value( key + "RadixChar", 'C' ).toString();
  if ( radixCharStr != "C" && radixCharStr != "," && radixCharStr != "." )
    radixChar = 'C';
  else
    radixChar = radixCharStr[0].toLatin1();

  saveSession    = settings->value( key + "RestoreLastSession", true  ).toBool();
  saveVariables  = settings->value( key + "SaveVariables",      true  ).toBool();
  autoComplete   = settings->value( key + "AutoComplete",       true  ).toBool();
  autoCalc       = settings->value( key + "AutoCalc",           true  ).toBool();
  minimizeToTray = settings->value( key + "MinimizeToTray",     false ).toBool();
  hiliteSyntax   = settings->value( key + "HiliteSyntax",       true  ).toBool();
  language       = settings->value( key + "Language",           "C"   ).toString();

  key = KEY + "/Format/";

  // format special case
  QString formatStr;
  formatStr = settings->value( key + "Type", 'g' ).toString();
  if (    formatStr != "g" && formatStr != "f" && formatStr != "e"
       && formatStr != "n" && formatStr != "h" && formatStr != "o"
       && formatStr != "b" )
    format = 'g';
  else
    format = formatStr[0].toLatin1();

  precision = settings->value( key + "Precision", -1  ).toInt();

  if ( precision > DECPRECISION )
    precision = DECPRECISION;

  key = KEY + "/Layout/";
  showFullScreen        = settings->value( key + "ShowFullScreen",       false  ).toBool();
  showKeypad            = settings->value( key + "ShowKeypad",           true   ).toBool();
  showMenuBar           = settings->value( key + "ShowMenuBar",          true   ).toBool();
  showStatusBar         = settings->value( key + "ShowStatusBar",        false  ).toBool();
  showHistory           = settings->value( key + "ShowHistory",          false  ).toBool();
  showFunctions         = settings->value( key + "ShowFunctions",        false  ).toBool();
  showVariables         = settings->value( key + "ShowVariables",        false  ).toBool();
  showBook              = settings->value( key + "ShowBook",             false  ).toBool();
  showConstants         = settings->value( key + "ShowConstants",        false  ).toBool();
  mainWindowState       = settings->value( key + "State"                        ).toByteArray();
  stayAlwaysOnTop       = settings->value( key + "StayAlwaysOnTop",       false ).toBool();
  historyDockFloating   = settings->value( key + "HistoryDockFloating",   false ).toBool();
  historyDockTop        = settings->value( key + "HistoryDockTop",        0     ).toInt();
  historyDockLeft       = settings->value( key + "HistoryDockLeft",       0     ).toInt();
  historyDockWidth      = settings->value( key + "HistoryDockWidth",      150   ).toInt();
  historyDockHeight     = settings->value( key + "HistoryDockHeight",     350   ).toInt();
  functionsDockFloating = settings->value( key + "FunctionsDockFloating", false ).toBool();
  functionsDockTop      = settings->value( key + "FunctionsDockTop",      0     ).toInt();
  functionsDockLeft     = settings->value( key + "FunctionsDockLeft",     0     ).toInt();
  functionsDockWidth    = settings->value( key + "FunctionsDockWidth",    150   ).toInt();
  functionsDockHeight   = settings->value( key + "FunctionsDockHeight",   350   ).toInt();
  bookDockFloating      = settings->value( key + "BookDockFloating",      false ).toBool();
  bookDockTop           = settings->value( key + "BookDockTop",           0     ).toInt();
  bookDockLeft          = settings->value( key + "BookDockLeft",          0     ).toInt();
  bookDockWidth         = settings->value( key + "BookDockWidth",         150   ).toInt();
  bookDockHeight        = settings->value( key + "BookDockHeight",        350   ).toInt();
  variablesDockFloating = settings->value( key + "VariablesDockFloating", false ).toBool();
  variablesDockTop      = settings->value( key + "VariablesDockTop",      0     ).toInt();
  variablesDockLeft     = settings->value( key + "VariablesDockLeft",     0     ).toInt();
  variablesDockWidth    = settings->value( key + "VariablesDockWidth",    150   ).toInt();
  variablesDockHeight   = settings->value( key + "VariablesDockHeight",   350   ).toInt();
  constantsDockFloating = settings->value( key + "ConstantsDockFloating", false ).toBool();
  constantsDockTop      = settings->value( key + "ConstantsDockTop",      0     ).toInt();
  constantsDockLeft     = settings->value( key + "ConstantsDockLeft",     0     ).toInt();
  constantsDockWidth    = settings->value( key + "ConstantsDockWidth",    150   ).toInt();
  constantsDockHeight   = settings->value( key + "ConstantsDockHeight",   350   ).toInt();
  mainWindowSize        = QSize( settings->value ( key + "WindowWidth",   300   ).toInt(),
                                 settings->value ( key + "WindowHeight",  400   ).toInt() );

  // load history
  key = KEY + "/History/";
  history.clear();
  int count = settings->value( key + "Count", 0 ).toInt();
  for ( int i = 0; i < count; i++ )
  {
    QString keyname = QString( key + "Expression%1" ).arg( i );
    QString str = settings->value( keyname ).toString();
    if ( ! str.isEmpty() )
    {
      QString expr;
      for ( int c = 0; c < str.length(); c++ )
        if ( str[c] >= 32 )
          expr.append( str[c] );
      history.append( expr );
    }
  }
  // load results
  historyResults.clear();
  for ( int i = 0; i < count; i++ )
  {
    QString keyname = QString( key + "Expression%1Result" ).arg( i );
    QString str = settings->value( keyname ).toString();
    if ( ! str.isEmpty() )
      historyResults.append( str );
  }
  if ( history.count() != historyResults.count() )
  {
    // avoid application crash because of new features with old settings files
    history.clear();
    historyResults.clear();
  }

  // load variables
  key = KEY + "/Variables/";
  variables.clear();
  settings->beginGroup( key );
  QStringList names = settings->childKeys();
  settings->endGroup();
  for ( int k = 0; k < names.count(); k++ )
  {
    QString name = names[k];
    QString keyname = QString( key + "%1" ).arg( name );
    QString value = settings->value( keyname ).toString();
    // treat upper case escape code
    int length = name.length();
    for ( int c = 0; c < length; c++ )
    {
      if ( name[c] == '_' )
      {
        name.remove( c, 1 );
        if ( name[c] != '_' )
          name[c] = name[c].toUpper();
      }
    }
    // load
    if ( ! value.isEmpty() && name != "pi" && name != "phi" )
      variables.append( QString( "%1=%2" ).arg( name ).arg( value ) );
  }

  delete settings;
}


void SC_Settings::save()
{
  const QString KEY = "SpeedCrunch";

  QSettings * settings = createQSettings( KEY );
  if ( ! settings )
    return;

  QString key;
  int k, i;

  key = KEY + "/General/";

  settings->setValue( key + "AngleMode",          QString( QChar( angleMode ) ) );
  settings->setValue( key + "RadixChar",          QString( QChar( radixChar ) ) );
  settings->setValue( key + "RestoreLastSession", saveSession    );
  settings->setValue( key + "SaveVariables",      saveVariables  );
  settings->setValue( key + "AutoComplete",       autoComplete   );
  settings->setValue( key + "AutoCalc",           autoCalc       );
  settings->setValue( key + "MinimizeToTray",     minimizeToTray );
  settings->setValue( key + "HiliteSyntax",       hiliteSyntax   );
  settings->setValue( key + "Language",           language       );

  key = KEY + "/Format/";

  settings->setValue( key + "Type",      QString( QChar( format ) ) );
  settings->setValue( key + "Precision", precision );

  key = KEY + "/Layout/";

  settings->setValue( key + "ShowBook",              showBook                );
  settings->setValue( key + "ShowConstants",         showConstants           );
  settings->setValue( key + "ShowFunctions",         showFunctions           );
  settings->setValue( key + "ShowHistory",           showHistory             );
  settings->setValue( key + "ShowFullScreen",        showFullScreen          );
  settings->setValue( key + "ShowKeypad",            showKeypad              );
  settings->setValue( key + "ShowMenuBar",           showMenuBar             );
  settings->setValue( key + "ShowStatusBar",         showStatusBar           );
  settings->setValue( key + "ShowVariables",         showVariables           );
  settings->setValue( key + "WindowHeight",          mainWindowSize.height() );
  settings->setValue( key + "WindowWidth",           mainWindowSize.width()  ),
  settings->setValue( key + "ConstantsDockFloating", constantsDockFloating   );
  settings->setValue( key + "ConstantsDockHeight",   constantsDockHeight     );
  settings->setValue( key + "ConstantsDockLeft",     constantsDockLeft       );
  settings->setValue( key + "ConstantsDockTop",      constantsDockTop        );
  settings->setValue( key + "ConstantsDockWidth",    constantsDockWidth      );
  settings->setValue( key + "FunctionsDockFloating", functionsDockFloating   );
  settings->setValue( key + "FunctionsDockHeight",   functionsDockHeight     );
  settings->setValue( key + "FunctionsDockLeft",     functionsDockLeft       );
  settings->setValue( key + "FunctionsDockTop",      functionsDockTop        );
  settings->setValue( key + "FunctionsDockWidth",    functionsDockWidth      );
  settings->setValue( key + "BookDockFloating",      bookDockFloating        );
  settings->setValue( key + "BookDockHeight",        bookDockHeight          );
  settings->setValue( key + "BookDockLeft",          bookDockLeft            );
  settings->setValue( key + "BookDockTop",           bookDockTop             );
  settings->setValue( key + "BookDockWidth",         bookDockWidth           );
  settings->setValue( key + "HistoryDockFloating",   historyDockFloating     );
  settings->setValue( key + "HistoryDockHeight",     historyDockHeight       );
  settings->setValue( key + "HistoryDockLeft",       historyDockLeft         );
  settings->setValue( key + "HistoryDockTop",        historyDockTop          );
  settings->setValue( key + "HistoryDockWidth",      historyDockWidth        );
  settings->setValue( key + "State",                 mainWindowState         );
  settings->setValue( key + "StayAlwaysOnTop",       stayAlwaysOnTop         );
  settings->setValue( key + "VariablesDockFloating", variablesDockFloating   );
  settings->setValue( key + "VariablesDockHeight",   variablesDockHeight     );
  settings->setValue( key + "VariablesDockLeft",     variablesDockLeft       );
  settings->setValue( key + "VariablesDockTop",      variablesDockTop        );
  settings->setValue( key + "VariablesDockWidth",    variablesDockWidth      );

  // save history
  key = KEY + "/History/";
  QStringList realHistory = history;
  QStringList realHistoryResults = historyResults;
  if ( history.count() > 100 )
  {
    realHistory.clear();
    realHistoryResults.clear();
    unsigned start = history.count() - 100;
    for ( int j = start; j < history.count(); j++ )
    {
      realHistory.append( history[j] );
      realHistoryResults.append( historyResults[j] );
    }
  }

  settings->beginGroup( key );
  QStringList hkeys = settings->childKeys();
  settings->endGroup();

  for ( k = 0; k < hkeys.count(); k++ )
  {
    settings->remove( QString( key + "Expression%1" ).arg( k )       );
    settings->remove( QString( key + "Expression%1Result" ).arg( k ) );
  }

  settings->setValue( key + "/Count/", (int) history.count() );
  for ( i = 0; i < realHistory.count(); i++ )
  {
    settings->setValue( QString( key + "Expression%1" ).arg( i ),       realHistory[i]        );
    settings->setValue( QString( key + "Expression%1Result" ).arg( i ), realHistoryResults[i] );
  }

  // save variables
  key = KEY + "/Variables/";
  settings->beginGroup( key );
  QStringList vkeys = settings->childKeys();
  settings->endGroup();

  for ( k = 0; k < vkeys.count(); k++ )
    settings->remove( QString( key + "%1" ).arg( vkeys[k] ) );

  for ( i = 0; i < variables.count(); i++ )
  {
    QStringList s = variables[i].split( '=' );

    if ( s.count() == 2 && s[0] != "pi" && s[0] != "phi" )
    {
      QString name  = "";
      QString value = s[1];
      int length = s[0].length();
      for ( int c = 0; c < length; c++ )
      {
        if ( s[0].at(c).isUpper() || s[0].at(c) == '_' )
        {
          name += '_';
          name += s[0][c].toLower();
        }
        else
        {
          name += s[0][c];
        }
      }
      settings->setValue( QString( key + "%1").arg( name ), value );
    }
  }

  delete settings;
}


QSettings * createQSettings( const QString & KEY )
{
  QSettings * settings = 0;

  #ifdef Q_WS_WIN
  #ifdef SPEEDCRUNCH_PORTABLE
    // Portable Windows version: settings are from INI file in the same directory
    QString appPath = QApplication::applicationFilePath();
    int ii = appPath.lastIndexOf( '/' );
    if ( ii > 0)
        appPath.remove(ii, appPath.length());
    QString iniFile = appPath + '/' + KEY + ".ini";
    settings = new QSettings( iniFile, QSettings::IniFormat );
  #else
    // Regular Windows version:
    //   settings are from the registry HKEY_CURRENT_USER\Software\SpeedCrunch
    settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope,
                              KEY, KEY );
  #endif // SPEEDCRUNCH_PORTABLE
  #endif // Q_WS_WIN


  #ifdef Q_WS_MAC
    settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope,
                              KEY, KEY );
  #endif // Q_WS_MAC


  #ifdef Q_WS_X11
  #ifdef SPEEDCRUNCH_PORTABLE
    // Portable X11 version: settings are from INI file in the same directory
    BrInitError error;
    if ( br_init( & error ) == 0 && error != BR_INIT_ERROR_DISABLED )
    {
        qDebug( "Warning: BinReloc failed to initialize (error code %d)",
                error );
        qDebug( "Will fallback to hardcoded default path." );
    }

    char * prefix = br_find_prefix( 0 );
    QString iniFile = QString( prefix ) + '/' + KEY + ".ini";
    free( prefix );
    settings = new QSettings( iniFile, QSettings::IniFormat );
  #else
    // Regular Unix (not Mac) version: settings from $HOME/.conf/SpeedCrunch
    settings = new QSettings( QSettings::NativeFormat, QSettings::UserScope,
                              KEY, KEY );
  #endif // SPEEDCRUNCH_PORTABLE
  #endif // Q_WS_X11

  return settings;
}
