#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    CellInterface::Value operator() (Position pos);

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

    // Можете дополнить ваш класс нужными полями и методами

private:
    struct PositionHasher {
        std::size_t operator()(const Position& value) const {
            using std::hash;
            return hash<int>()(value.row) ^ hash<int>()(value.col);
        }
    };
    std::unordered_map<Position, std::unique_ptr<Cell>, PositionHasher> sheet_;
    std::map<int, int> row_to_pos_index_;
    std::map<int, int> col_to_pos_index_;

    void CheckValidity(Position pos) const;
};
