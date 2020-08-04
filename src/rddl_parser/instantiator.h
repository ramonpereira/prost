#ifndef INSTANTIATOR_H
#define INSTANTIATOR_H

#include <vector>

namespace prost {
namespace parser {
class Parameter;
class ParametrizedVariable;
class LogicalExpression;
struct RDDLTask;

class Instantiator {
public:
    Instantiator(RDDLTask* _task) : task(_task) {}

    void instantiate(bool const& output = true);
    void instantiateParams(
        std::vector<Parameter*> params,
        std::vector<std::vector<Parameter*>>& result,
        std::vector<Parameter*> addTo = std::vector<Parameter*>(),
        int indexToProcess = 0);

private:
    RDDLTask* task;

    void instantiateVariables();
    void instantiateCPFs();
    void instantiateCPF(ParametrizedVariable* head, LogicalExpression* formula);
    void instantiatePreconds();
};
} // namespace parser
} // namespace prost

#endif
