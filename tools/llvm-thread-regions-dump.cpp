#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_os_ostream.h>

#if LLVM_VERSION_MAJOR >= 4
#include <llvm/Bitcode/BitcodeReader.h>
#else
#include <llvm/Bitcode/ReaderWriter.h>
#endif

#include <fstream>
#include <iostream>

#include "dg/PointerAnalysis/PointerAnalysisFI.h"
#include "dg/llvm/PointerAnalysis/PointerAnalysis.h"
#include "dg/llvm/ThreadRegions/ControlFlowGraph.h"
#include "llvm/ThreadRegions/Graphs/GraphBuilder.h"
#include "llvm/ThreadRegions/Graphs/ThreadRegionsBuilder.h"

#include <iostream>

using namespace std;

int main(int argc, const char *argv[]) {
    using namespace llvm;

    cl::opt<string> OutputFilename("o", cl::desc("Specify output filename"),
                                   cl::value_desc("filename"), cl::init(""));
    cl::opt<std::string> inputFile(cl::Positional, cl::Required,
                                   cl::desc("<input file>"), cl::init(""));

    cl::ParseCommandLineOptions(argc, argv);

    std::string module = inputFile;
    std::string graphvizFileName = OutputFilename;

    llvm::LLVMContext context;
    llvm::SMDiagnostic SMD;

    std::unique_ptr<Module> M = llvm::parseIRFile(module, SMD, context);

    if (!M) {
        llvm::errs() << "Failed parsing '" << module << "' file:\n";
        SMD.print(argv[0], errs());
        return 1;
    }

    dg::DGLLVMPointerAnalysis pointsToAnalysis(M.get(), "main",
                                               dg::Offset::UNKNOWN, true);
    pointsToAnalysis.run();

    ControlFlowGraph controlFlowGraph(&pointsToAnalysis);
    controlFlowGraph.buildFunction(M->getFunction("main"));

    if (graphvizFileName.empty()) {
        controlFlowGraph.printWithRegions(std::cout);
    } else {
        std::ofstream graphvizFile(graphvizFileName);
        controlFlowGraph.printWithRegions(graphvizFile);
    }

    return 0;
}
