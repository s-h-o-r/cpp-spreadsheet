#include "cell.h"

#include <cassert>
#include <forward_list>
#include <iostream>
#include <string>
#include <optional>

// Реализуйте следующие методы
class Cell::Impl {
public:
    virtual ~Impl() = default;

    virtual CellInterface::Value GetValue(const Cell* cell) const = 0;
    virtual std::string GetText() const = 0;

    virtual std::vector<Position> GetReferencedCells() const = 0;
};

class Cell::EmptyImpl final : public Cell::Impl {
public:
    EmptyImpl() = default;

    CellInterface::Value GetValue(const Cell*) const override {
        return {};
    }

    std::string GetText() const override {
        return {};
    }

    std::vector<Position> GetReferencedCells() const override {
        return {};
    }

};

class Cell::TextImpl final : public Cell::Impl {
public:
    explicit TextImpl(std::string expression)
        : text_(expression) {
    }

    CellInterface::Value GetValue(const Cell*) const override {
        if (text_[0] == '\'') {
            return std::string(text_.begin() + 1, text_.end());
        }
        return text_;
    }

    std::string GetText() const override {
        return text_;
    }

    std::vector<Position> GetReferencedCells() const override {
        return {};
    }

private:
    std::string text_;
};

class Cell::FormulaImpl final : public Cell::Impl {
public:
    explicit FormulaImpl(std::string expression) 
        : formula_(ParseFormula(std::move(expression))) {
    }

    CellInterface::Value GetValue(const Cell* cell) const override {
        auto formula_result = formula_->Evaluate(cell->sheet_);
        if (std::holds_alternative<double>(formula_result)) {
            return std::get<double>(formula_result);
        } else {
            return std::get<FormulaError>(formula_result);
        }
    }

    std::string GetText() const override {
        return std::string(FORMULA_SIGN + formula_->GetExpression());
    }

    std::vector<Position> GetReferencedCells() const override {
        return formula_->GetReferencedCells();
    }

private:
    std::unique_ptr<FormulaInterface> formula_;
};

Cell::Cell(SheetInterface& sheet)
    : impl_(std::make_unique<EmptyImpl>())
    , sheet_(sheet) {
}

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    if (GetText() == text) {
        return;
    }

    if (text.empty()) {
        impl_ = std::make_unique<EmptyImpl>();
        ClearDependencies();
    } else if (text[0] == FORMULA_SIGN && text.size() > 1) {
        text.erase(text.begin());
        std::unique_ptr<Impl> formula_impl = std::make_unique<FormulaImpl>(std::move(text));
        std::vector<Position> ref_cells_pos = formula_impl->GetReferencedCells();

        CheckCyclicDependencies(this, ref_cells_pos);

        impl_ = std::move(formula_impl);

        SetDependencies(ref_cells_pos);
    } else {
        impl_ = std::make_unique<TextImpl>(std::move(text));
        ClearDependencies();
    }
    ResetCache();
}

void Cell::Clear() {
    Set("");
}

CellInterface::Value Cell::GetValue() const {
    if (!cache_.has_value()) {
        cache_.emplace(impl_->GetValue(this));
    }
    return cache_.value();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
    return !references_from_.empty();
}

void Cell::SetReference(Cell* referenced_cell) {
    references_to_.insert(referenced_cell);
    referenced_cell->references_from_.insert(this);
}

void Cell::ClearDependencies() {
    if (!references_to_.empty()) {
        for (Cell* ref_cell : references_to_) {
            ref_cell->references_from_.erase(this);
        }
        references_to_.clear();
    }
}

void Cell::SetDependencies(const std::vector<Position>& ref_cells_pos) {
    ClearDependencies();
    for (const Position& pos : ref_cells_pos) {
        auto cell = sheet_.GetCell(pos);
        if (!cell) {
            sheet_.SetCell(pos, {});
            cell = sheet_.GetCell(pos);
        }
        SetReference(static_cast<Cell*>(cell));
    }
}

void Cell::ResetCache() {
    std::unordered_set<Cell*> used;
    ResetCache(used);
}

void Cell::ResetCache(std::unordered_set<Cell*>& used) {
    cache_.reset();
    used.insert(this);
    for (Cell* referenced_cell : references_from_) {
        if (!used.count(referenced_cell)) {
            referenced_cell->ResetCache(used);
        }
    }
}

void Cell::CheckCyclicDependencies(const CellInterface* vertex, std::vector<Position>& ref_cells_pos) const {
    std::unordered_set<const CellInterface*> used;
    for (const auto& pos : ref_cells_pos) {
        if (sheet_.GetCell(pos) != nullptr) {
            CheckCyclicDependencies(vertex, sheet_.GetCell(pos), used);
        }
    }
}

void Cell::CheckCyclicDependencies(const CellInterface* checking_vertex, const CellInterface* vertex,
                                    std::unordered_set<const CellInterface*>& used) const {
    used.insert(vertex);
    if (checking_vertex == vertex) {
        throw CircularDependencyException("Circular dependency has been found");
    }
    std::vector<Position> referenced_cells_pos = vertex->GetReferencedCells();
    for (auto pos : referenced_cells_pos) {
        if (!used.count(sheet_.GetCell(pos)) && sheet_.GetCell(pos) != nullptr) {
            CheckCyclicDependencies(checking_vertex, sheet_.GetCell(pos), used);
        }
    }
}

