#include <pokedex_interface.h>

#include <cstdlib>
#include <cstdint>
#include <new>

#include "pokedex_util.h"
#include "model/model.h"

template<typename Model>
constexpr pokedex_model_description model_desc = {
    .model_isa = "rv32",
    .model_priv = "M",
    .xlen = Model::XLEN,
    .flen = Model::FLEN,
    .vlen = Model::VLEN,
};

static inline uint64_t sext64(uint32_t x) {
    return (uint64_t)(int64_t)(int32_t)x;
}

static inline uint64_t nanbox64(uint32_t x) {
    return (uint64_t(UINT32_MAX) << 32) | uint64_t(x);
}

template<typename Model>
struct model_export {
    static void* model_create(const pokedex_create_info* info, char* err_buf, size_t err_buflen) {
        // NOTE: use malloc + placement new to avoid linking with libstdc++
        static_assert(alignof(Model) <= 16);
        void* model_ = malloc(sizeof(Model));
        // placement new
        new (model_) Model;
        return model_;
    }

    static void model_destroy(void* model_) {
        if (model_) {
            ((Model*)model_)->~Model();
            free(model_);
        }
    }

    static const pokedex_model_description* model_get_description([[maybe_unused]] void* model_) {
        return &model_desc<Model>;
    }

    static void model_read_pc(void* model_, uint64_t* ret) {
        Model* model = (Model*)model_;
        if constexpr (Model::XLEN == 32) {
            *ret = sext64(model->pc());
        }
        else if constexpr (Model::XLEN == 64) {
            *ret = model->pc();
        }
        else {
            static_assert(false);
        }
    }

    static void model_read_xreg(void* model_, uint8_t xs_, uint64_t* ret) {
        Model* model = (Model*)model_;
        XRegIdx xs = xreg_from_idx(xs_);
        if constexpr (Model::XLEN == 32) {
            *ret = sext64(model->xreg(xs));
        }
        else if constexpr (Model::XLEN == 64) {
            *ret = model->xreg(xs);
        }
        else {
            static_assert(false);
        }
    }

    static void model_read_freg(void* model_, uint8_t fs_, uint64_t* ret) {
        Model* model = (Model*)model_;
        FRegIdx fs = freg_from_idx(fs_);
        if constexpr (Model::FLEN == 32) {
            *ret = nanbox64(model->freg(fs));
        }
        else if constexpr (Model::FLEN == 64) {
            *ret = model->freg(fs);
        }
        else {
            static_assert(Model::FLEN == 0);
            abort();
        }
    }

    static void model_read_csr(void* model_, uint16_t csr_idx, uint64_t* ret) {
        Model* model = (Model*)model_;
        CsrIdx csr = csr_from_idx(csr_idx);

        *ret = model->csr(csr);
    }

    static void model_reset(void* model_, uint32_t initial_pc) {
        Model* model = (Model*)model_;
        model->reset(initial_pc);
    }

    static uint8_t model_step_trace(
        void* model_,
        const pokedex_mem_callback_vtable* mem_callback_vtable,
        void* mem_callback_data
    ) {
        Model* model = (Model*)model_;

        StepResult res = model->step_trace(MemCallback {
            .m_vtable = mem_callback_vtable,
            .m_data = mem_callback_data,
        });
        return res.m_code;
    }

    static const pokedex_trace_buffer* model_get_trace_buffer(void* model_) {
        Model* model = (Model*)model_;

        return model->trace.get_buffer();
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

