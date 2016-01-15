#ifdef _WIN32
#include "osapi.h"

#include <Windows.h>
#include <Winspool.h>
#include <QString>
#include <QProcess>
#include <QApplication>

/*
BOOL RawDataToPrinter(LPTSTR docName, LPTSTR szPrinterName, LPBYTE lpData, DWORD dwCount)
{
    BOOL     bStatus = FALSE;
    HANDLE     hPrinter = NULL;
    DOC_INFO_1 DocInfo;
    DWORD      dwJob = 0L;
    DWORD      dwBytesWritten = 0L;

    // Open a handle to the printer.
    bStatus = OpenPrinter( szPrinterName, &hPrinter, NULL );
    if (bStatus) {
        // Fill in the structure with info about this "document."
        DocInfo.pDocName = docName;
        DocInfo.pOutputFile = NULL;
        DocInfo.pDatatype = (LPTSTR)L"RAW";

        // Inform the spooler the document is beginning.
        dwJob = StartDocPrinter( hPrinter, 1, (LPBYTE)&DocInfo );
        if (dwJob > 0) {
            // Start a page.
            bStatus = StartPagePrinter( hPrinter );
            if (bStatus) {
                // Send the data to the printer.
                bStatus = WritePrinter( hPrinter, lpData, dwCount, &dwBytesWritten);
                EndPagePrinter (hPrinter);
            }
            // Inform the spooler that the document is ending.
            EndDocPrinter( hPrinter );
        }
        // Close the printer handle.
        ClosePrinter( hPrinter );
    }
    // Check to see if correct number of bytes were written.
    if (!bStatus || (dwBytesWritten != dwCount)) {
        bStatus = FALSE;
    } else {
        bStatus = TRUE;
    }
    return bStatus;
}
*/

std::wstring s2ws(const std::string& s)
{
 int len;
 int slength = (int)s.length() + 1;
 len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
 wchar_t* buf = new wchar_t[len];
 MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
 std::wstring r(buf);
 delete[] buf;
 return r;
}


//BSTR QStringToBSTR(const QString &qstring)
//{
//    BSTR result = SysAllocStringLen(0, qstring.length());
//    qstring.toWCharArray(result);
//    return result;
//}


QString OSApi::AppDir()
{
    return qApp->applicationDirPath() + QString("/");
}


QString OSApi::ResourcesDir()
{
    return qApp->applicationDirPath() + QString("/");
}


void OSApi::PrintFileTo(QString printer, QString file)
{
    printer.prepend("\"");printer.append("\"");
    file = file.trimmed().replace("/", "\\");
    std::wstring sTemp = s2ws(file.toStdString()); // Temporary buffer is required
    LPCWSTR wFile = sTemp.c_str();
    std::wstring sTemp2 = s2ws(printer.trimmed().toStdString()); // Temporary buffer is required
    LPCWSTR wPrinter = sTemp2.c_str();
    SHELLEXECUTEINFO shExecInfo;
    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.fMask = NULL;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = L"printto";
    shExecInfo.lpFile =  wFile;
    shExecInfo.lpParameters = wPrinter;
    shExecInfo.lpDirectory = NULL;
    shExecInfo.nShow = SW_HIDE;
    shExecInfo.hInstApp = NULL;
    ::ShellExecuteEx(&shExecInfo);
    //int retVal = ::GetLastError();
}


//void WinApi::PrintFileTo2(QString printer, QString file)
//{
//    QStringList args;
//    args << "print"; args << printer; args << file.replace("/", "\\"); //   = QString("printskrnlwrtr%1skrnlwrtr%2").arg(printer).arg(file);
//    QProcess::startDetached("printtool.exe", args);
//}



//void WinApi::PrintFileTo2(QString printer, QString file)
//{
//	HRESULT hr = CoInitialize(NULL);
//    printer.prepend("\"");printer.append("\"");
//    file = file.trimmed().replace("/", "\\");
//	ICOMPrinterPtr com(__uuidof(COMPrinter));
//	BSTR bPrinter = QStringToBSTR(printer), bFile = QStringToBSTR(file);
//	com->PrintFile(bPrinter, bFile);
//}


void OSApi::ExecuteInstallerSilently(const QString& filename)
{
    QStringList args; args <<"/VERYSILENT" <<"/SP-" <<"/SUPPRESSMSGBOXES" <<"/CLOSEAPPLICATIONS";
    QProcess::startDetached(filename, args);
}

#endif

