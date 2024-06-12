#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <optional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
    explicit Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    CellInterface::Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    std::unique_ptr<Impl> impl_;

    std::unordered_set<Cell*> references_to_;
    std::unordered_set<Cell*> references_from_;
    mutable std::optional<CellInterface::Value> cache_;

    SheetInterface& sheet_;

    void SetReference(Cell* referenced_cell);
    void ClearDependancies();
    void SetFormulaImpl(std::string text);
    void ResetCache();
    void ResetCache(std::unordered_set<Cell*>& used);

    void CheckCyclicDependencies(const CellInterface* vertex, std::vector<Position>& ref_cells) const;
    void CheckCyclicDependencies(const CellInterface* checking_vertex, const CellInterface* vertex,
                                 std::unordered_set<const CellInterface*>& used) const;
};
