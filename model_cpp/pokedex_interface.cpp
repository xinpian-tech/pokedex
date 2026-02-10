#include <pokedex_interface.h>

#include <cstdlib>
#include <cstdint>
#include <new>

#include "model/model.h"

// template<typename Model>
// constexpr pokedex_model_description model_desc = {
//     .model_isa = "rv32",
//     .model_priv = "M",
//     .xlen = Model::XLEN,
//     .flen = Model::FLEN,
//     .vlen = Model::VLEN,
// };

static inline uint64_t sext64(uint32_t x) {
    return (uint64_t)(int64_t)(int32_t)x;
}

static inline uint64_t nanbox64(F32 x) {
    return (uint64_t(UINT32_MAX) << 32) | uint64_t(x.bits);
}

template<typename Model>
struct model_export {
    struct Wrapper {
        Model inner;
        TraceBuffer trace_buffer;
        pokedex_model_description description;

        Wrapper(int vlen):
            inner(vlen),
            description {
                .model_isa = "rv32",
                .model_priv = "M",
                .xlen = Model::XLEN,
                .flen = Model::FLEN,
                .vlen = (uint32_t)vlen,
            }
        {}
    };

    static void* model_create(const pokedex_create_info* info, char* err_buf, size_t err_buflen) {
        (void)info;
        (void)err_buf;
        (void)err_buflen;

        // NOTE: use malloc + placement new to avoid linking with libstdc++
        void* model_ = malloc(alignof(Wrapper), sizeof(Wrapper));
        if (!model_) {
            snprintf(err_buf, err_buflen, "malloc failed");
            return nullptr;
        }
        // placement new
        new (model_) Wrapper(256);
        return model_;
    }

    static void model_destroy(void* model_) {
        if (model_) {
            static_cast<Wrapper*>(model_)->~Wrapper();
            free(model_);
        }
    }

    static const pokedex_model_description* model_get_description(void* model_) {
        Wrapper* wrapper = (Wrapper*)model_;
        return &wrapper->description;
    }

    static void model_read_pc(void* model_, uint64_t* ret) {
        Wrapper* wrapper = (Wrapper*)model_;
        Model& model = wrapper->inner;
        if constexpr (Model::XLEN == 32) {
            *ret = sext64(model.pc());
        }
        else if constexpr (Model::XLEN == 64) {
            *ret = model.pc();
        }
        else {
            static_assert(false);
        }
    }

    static void model_read_xreg(void* model_, uint8_t xs_, uint64_t* ret) {
        Wrapper* wrapper = (Wrapper*)model_;
        Model& model = wrapper->inner;
        XRegIdx xs = xreg_from_idx(xs_);
        if constexpr (Model::XLEN == 32) {
            *ret = sext64(model.xreg(xs));
        }
        else if constexpr (Model::XLEN == 64) {
            *ret = model.xreg(xs);
        }
        else {
            static_assert(false);
        }
    }

    static void model_read_freg(void* model_, uint8_t fs_, uint64_t* ret) {
        Wrapper* wrapper = (Wrapper*)model_;
        Model& model = wrapper->inner;
        FRegIdx fs = freg_from_idx(fs_);
        if constexpr (Model::FLEN == 32) {
            *ret = nanbox64(model.freg(fs));
        }
        else if constexpr (Model::FLEN == 64) {
            *ret = model.freg(fs);
        }
        else {
            static_assert(Model::FLEN == 0);
            abort();
        }
    }

    static void model_read_csr(void* model_, uint16_t csr_idx, uint64_t* ret) {
        Wrapper* wrapper = (Wrapper*)model_;
        Model& model = wrapper->inner;
        CsrIdx csr = csr_from_idx(csr_idx);

        *ret = model.csr(csr);
    }

    static void model_reset(void* model_, uint32_t initial_pc) {
        Wrapper* wrapper = (Wrapper*)model_;
        Model& model = wrapper->inner;
        model.reset(initial_pc);
    }

    static uint8_t model_step_trace(
        void* model_,
        const pokedex_mem_callback_vtable* mem_callback_vtable,
        void* mem_callback_data
    ) {
        Wrapper* wrapper = (Wrapper*)model_;
        Model& model = wrapper->inner;

        MemCallback mem_cb = {
            .m_vtable = mem_callback_vtable,
            .m_data = mem_callback_data,
        };

        StepResult res = model.step_trace(mem_cb, &wrapper->trace_buffer);
        return res.m_code;
    }

    static const pokedex_trace_buffer* model_get_trace_buffer(void* model_) {
        Wrapper* wrapper = (Wrapper*)model_;

        return wrapper->trace_buffer.get_buffer();
    }

    static constexpr pokedex_model_export EXPORT_TABLE = {
        .abi_version = POKEDEX_ABI_VERSION,
        .create = model_create,
        .destroy = model_destroy,

        .get_description = model_get_description,

        .reset = model_reset,
        .step = model_step_trace,
        .step_trace = model_step_trace,
        .get_trace_buffer = model_get_trace_buffer,

        .get_pc = model_read_pc,
        .get_xreg = model_read_xreg,
        .get_freg = model_read_freg,
        // .get_vreg = model_read_vreg,
        .get_csr = model_read_csr,
    };
};


extern "C" __attribute__((visibility("default")))
const pokedex_model_export* EXPORT_pokedex_get_model_export() {
  return &model_export<CoreModel>::EXPORT_TABLE;
}

