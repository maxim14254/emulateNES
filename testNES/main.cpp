#include <iostream>
#include <QFile>
#include <QDir>
#include <QTextStream>



using namespace std;

int main()
{
    QString path_log = QString("%1/log.txt").arg(QDir::currentPath());
    QFile file_log(path_log);

    QString path_benchmark = QString("%1/benchmark_log.txt").arg(QDir::currentPath());
    QFile file_benchmark(path_benchmark);

    if (file_log.open(QIODevice::ReadOnly) && file_benchmark.open(QIODevice::ReadOnly))
    {
        QTextStream TS_log(&file_log);

        QTextStream TS_benchmark(&file_benchmark);

        int count_lines = 1;
        while (!TS_log.atEnd() && !TS_benchmark.atEnd())
        {
            QString line_log = TS_log.readLine();

            QString line_benchmark = TS_benchmark.readLine();

            int j = 0;
            int i = 0;
            for(; i < line_benchmark.size() && j < line_log.size(); j = ++i )
            {
                if(i == 73)
                    i += 12;

                if(line_benchmark[i] != line_log[j])
                {
                    cout << "Несовподение в строке:" << count_lines << " символ №" << j << " " << line_log[j].toLatin1() << "!=" << line_benchmark[i].toLatin1() << std::endl;
                    return 0;
                }
                //73
            }

            ++count_lines;
        }
    }
    else
    {
        cout << "Не удалось открыть логи\n";
    }

    return 0;
}
