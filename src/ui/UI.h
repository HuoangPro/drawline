#ifndef UI_H
#define UI_H
#include <QObject>
#include "RobotDraw.h"
class UI : public QObject {
    Q_OBJECT
    Q_PROPERTY(int dimension READ dimension CONSTANT)
    Q_PROPERTY(std::vector<std::vector<int>> grid READ grid CONSTANT)
    Q_PROPERTY(std::vector<std::vector<int>> lines READ lines CONSTANT)
public:
    UI(Grid& g) : m_grid(g){
        qRegisterMetaType<std::vector<std::vector<int>> >("std::vector<std::vector<int>>");
    }

    int dimension() const {
        return m_grid.getDimension();
    }

    std::vector<std::vector<int>> grid() const {
        return m_grid.getData();
    }

    std::vector<std::vector<int>> lines() const {
        return m_grid.getLines();
    }

private:
    Grid& m_grid;
};

#endif // UI_H
