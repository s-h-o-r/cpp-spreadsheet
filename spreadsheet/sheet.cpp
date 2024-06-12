#include "sheet.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>

using namespace std::literals;

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value) {
    std::visit(
        [&](const auto& x) {
            output << x;
        },
        value);
    return output;
}

Sheet::~Sheet() = default;

void Sheet::SetCell(Position pos, std::string text) {
    CheckValidity(pos);
    
    if (!sheet_.count(pos)) {
        sheet_[pos] = std::make_unique<Cell>(*this);
    }

    sheet_[pos]->Set(text);
    row_to_pos_index_[pos.row] += 1;
    col_to_pos_index_[pos.col] += 1;
}

const CellInterface* Sheet::GetCell(Position pos) const {
    CheckValidity(pos);
    if (sheet_.count(pos)) {
        return sheet_.at(pos).get();
    }
    return nullptr;
}
CellInterface* Sheet::GetCell(Position pos) {
    CheckValidity(pos);
    if (sheet_.count(pos)) {
        return sheet_.at(pos).get();
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos) {
    CheckValidity(pos);
    if (sheet_.count(pos)) {
        if (!sheet_.at(pos)->IsReferenced()) {
            sheet_.erase(pos);
        } else {
            sheet_.at(pos)->Clear();
        }

        row_to_pos_index_[pos.row] -= 1;
        if (row_to_pos_index_[pos.row] == 0) {
            row_to_pos_index_.erase(pos.row);
        }
        col_to_pos_index_[pos.col] -= 1;
        if (col_to_pos_index_[pos.col] == 0) {
            col_to_pos_index_.erase(pos.col);
        }
    }
}

Size Sheet::GetPrintableSize() const {
    if (sheet_.empty()) {
        return {0, 0};
    }
    auto row_iter = std::next(row_to_pos_index_.begin(), row_to_pos_index_.size() - 1);
    auto col_iter = std::next(col_to_pos_index_.begin(), col_to_pos_index_.size() - 1);
    return {row_iter->first + 1, col_iter->first + 1}; // достаем из map'ов последние индексы и прибавляем 1, чтобы получить количество row and col
}

void Sheet::PrintValues(std::ostream& output) const {
    Size print_size = GetPrintableSize();
    for (int row = 0; row != print_size.rows; ++row) {
        for (int col = 0; col != print_size.cols; ++col) {
            if (sheet_.count({row, col})) {
                output << GetCell({row, col})->GetValue();
            }
            if (col != print_size.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    Size print_size = GetPrintableSize();
    for (int row = 0; row != print_size.rows; ++row) {
        for (int col = 0; col != print_size.cols; ++col) {
            if (sheet_.count({row, col})) {
                output << GetCell({row, col})->GetText();
            }
            if (col != print_size.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}

void Sheet::CheckValidity(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Position is invalid");
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
