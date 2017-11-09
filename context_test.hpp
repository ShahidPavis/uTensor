#ifndef UTENSOR_CONTEXT_TESTS
#define UTENSOR_CONTEXT_TESTS

#include "mbed.h"
#include "uTensor_util.hpp"
#include "tensor.hpp"
#include "context.hpp"
#include "tensorIdxImporter.hpp"
#include "MatrixOps.hpp"
#include "test.hpp"



class contextTest : public Test {

  TensorIdxImporter t_import;

public:

  void MatMalTest(void) {
    testStart("Context QntMatMal Op");
    Context ctx;
    //inputs
    auto a =
        ctx.add(t_import.ubyte_import("/fs/testData/qMatMul/in/qA_0.idx"));
    auto a_min =
        ctx.add(t_import.float_import("/fs/testData/qMatMul/in/qA_1.idx"));
    auto a_max =
        ctx.add(t_import.float_import("/fs/testData/qMatMul/in/qA_2.idx"));
    auto b =
        ctx.add(t_import.ubyte_import("/fs/testData/qMatMul/in/qB_0.idx"));
    auto b_min =
        ctx.add(t_import.float_import("/fs/testData/qMatMul/in/qB_1.idx"));
    auto b_max =
        ctx.add(t_import.float_import("/fs/testData/qMatMul/in/qB_2.idx"));

    // reference outputs
    auto c =
        ctx.add(t_import.int_import("/fs/testData/qMatMul/out/qMatMul_0.idx"));
    auto c_min =
        ctx.add(t_import.float_import("/fs/testData/qMatMul/out/qMatMul_1.idx"));
    auto c_max =
        ctx.add(t_import.float_import("/fs/testData/qMatMul/out/qMatMul_2.idx"));


    auto out_c = ctx.add(new RamTensor<int>(c.lock()->getShape()));
    auto out_min = ctx.add(new RamTensor<float>(c_min.lock()->getShape()));
    auto out_max = ctx.add(new RamTensor<float>(c_max.lock()->getShape()));

    TList inputs = {a, a_min, a_max, b, b_min, b_max};
    TList outputs = {out_c, out_min, out_max};

    //set which tensors to keep alive
    auto ref_c_rptr = c.lock();
    auto ref_min_rptr = c_min.lock();
    auto ref_max_rptr = c_max.lock();
    auto out_c_rptr = out_c.lock();
    auto out_min_rptr = out_min.lock();
    auto out_max_rptr = out_max.lock();
    

    timer_start();
    ctx.push(new QntMatMulOp(), inputs, outputs);
    ctx.eval();
    timer_stop();

    double result = meanPercentErr<int>(ref_c_rptr.get(), out_c_rptr.get()) + meanPercentErr<float>(ref_min_rptr.get(), out_min_rptr.get()) +
                      meanPercentErr<float>(ref_max_rptr.get(), out_max_rptr.get());

    passed(result == 0);
  }
/*
  void RefCountTest(void) {
    testStart("Context Ref Count");
    //inputs
    Tensor* a = new RamTensor<int>({1,1,1});
    Tensor* b = new RamTensor<int>({1,1,1});
    Tensor* c = new RamTensor<int>({1,1,1});

    // reference outputs
    Tensor* out = new RamTensor<int>({1,1,1});
    out->keep_alive(true);


    Context ctx;
    timer_start();

    TList input0 = {a, b};
    TList output0 = {c};
    ctx.push(new AddOp(), inputs0, outputs0);

    TList input1 = {c, a};
    TList output1 = {b};
    ctx.push(new AddOp(), inputs1, outputs1);

    TList input2 = {a, b};
    TList output2 = {out};
    ctx.push(new AddOp(), inputs2, outputs2);
    ctx.eval();
    timer_stop();

    if(a != nullptr || b != nullptr || c != nullptr) {
        failed();
        return;
    }

    passed(out->read(0, 0) != 1);

  }
  */

  void runAll(void) {
    MatMalTest();
    //RefCountTest();
  }
};

#endif  // UTENSOR_IDX_IMPORTER_TESTS