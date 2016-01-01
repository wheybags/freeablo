#ifndef PROCESSINVOKER_HPP
#define PROCESSINVOKER_HPP

#include <QStringList>
#include <QString>
#include <QProcess>

namespace Launcher
{
    class ProcessInvoker : public QObject
    {
        Q_OBJECT

    public:

        ProcessInvoker();
        ~ProcessInvoker();

        QProcess* getProcess();

        inline bool startProcess(const QString &name, bool detached = false) { return startProcess(name, QStringList(), detached); }
        bool startProcess(const QString &name, const QStringList &arguments, bool detached = false);

    private:
        QProcess *mProcess;

        QString mName;
        QStringList mArguments;

    private slots:
        void processError(QProcess::ProcessError error);
        void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

    };
}

#endif // PROCESSINVOKER_HPP
