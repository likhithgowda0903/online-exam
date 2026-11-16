#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "models/Result.h"
#include "models/ExamSession.h"
#include <memory>

/**
 * @file Evaluator.h
 * @brief Evaluation strategy interface and implementation for scoring examinations.
 * 
 * Demonstrates:
 * - Strategy Design Pattern: Encapsulates scoring algorithms behind an abstract interface.
 * - Abstraction & Polymorphism: Allows future scoring variants (e.g., negative marking) without modifying the exam engine.
 */
class IEvaluationStrategy {
public:
    virtual ~IEvaluationStrategy() = default;
    virtual Result evaluate(const ExamSession& session) = 0;
};

class StandardEvaluator : public IEvaluationStrategy {
public:
    StandardEvaluator() = default;
    ~StandardEvaluator() override = default;

    Result evaluate(const ExamSession& session) override;
};

using EvaluationStrategyPtr = std::unique_ptr<IEvaluationStrategy>;

#endif // EVALUATOR_H
