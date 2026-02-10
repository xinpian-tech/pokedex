#pragma once

#include <pokedex_interface.h>

#include <type_traits>
#include <memory>
#include <cstring>
#include <cassert>

#include "model_helper.h"

struct StepResult {
    uint8_t m_code;
    uint32_t m_inst;

    static StepResult fetch_xcpt(){
        return {
            .m_code = POKEDEX_STEP_RESULT_FETCH_XCPT,
            .m_inst = 0,
        };
    }

    // a non-compreseed instruction commits
    static StepResult commit(Inst inst) {
        return {
            .m_code = POKEDEX_STEP_RESULT_INST_COMMIT,
            .m_inst = inst.data,
        };
    }

    // a compreseed instruction commits
    static StepResult commit_c(CInst inst) {
        return {
            .m_code = POKEDEX_STEP_RESULT_INST_COMMIT,
            .m_inst = inst.data,
        };
    }
};

struct MemCallback {
    const pokedex_mem_callback_vtable* m_vtable = nullptr;
    void* m_data = nullptr;

    MemCallback& operator=(std::nullptr_t) {
        m_vtable = nullptr;
        m_data = nullptr;
        return *this;
    }

    int inst_fetch_u16(uint32_t addr, uint16_t* ret) const {
        return m_vtable->inst_fetch_2(m_data, addr, ret);
    }

    template<typename T>
    int read(uint32_t addr, std::type_identity_t<T>* ret) const {
        if constexpr (std::is_same_v<T, uint8_t>) {
            return m_vtable->read_mem_1(m_data, addr, ret);
        }
        else if constexpr (std::is_same_v<T, uint16_t>) {
            return m_vtable->read_mem_2(m_data, addr, ret);
        }
        else if constexpr (std::is_same_v<T, uint32_t>) {
            return m_vtable->read_mem_4(m_data, addr, ret);
        }
        else static_assert(false);
    }

    template<typename T>
    int write(uint32_t addr, std::type_identity_t<T> value) const {
        if constexpr (std::is_same_v<T, uint8_t>) {
            return m_vtable->write_mem_1(m_data, addr, value);
        }
        else if constexpr (std::is_same_v<T, uint16_t>) {
            return m_vtable->write_mem_2(m_data, addr, value);
        }
        else if constexpr (std::is_same_v<T, uint32_t>) {
            return m_vtable->write_mem_4(m_data, addr, value);
        }
        else static_assert(false);
    }

    int amo_u32(uint32_t addr, uint8_t op, uint32_t src, uint32_t* ret) const {
        return m_vtable->amo_mem_4(m_data, addr, op, src, ret);
    }

    int lr_u32(uint32_t addr, uint32_t* ret) const {
        return m_vtable->lr_mem_4(m_data, addr, ret);
    }

    int sc_u32(uint32_t addr, uint32_t value, uint32_t* ret) const {
        return m_vtable->sc_mem_4(m_data, addr, value, ret);
    }
};

struct TraceBuffer {
    pokedex_trace_buffer m_inner;

    TraceBuffer() {
        m_inner.valid = 0;
    }

    const pokedex_trace_buffer* get_buffer() const {
        return &m_inner;
    }

    void begin_trace(uint32_t pc) {
        memset(&m_inner, 0, sizeof(m_inner));
        m_inner.valid = 1;
        m_inner.pc = pc;
    }

    void end_trace(StepResult res) {
        m_inner.step_status = res.m_code;
        m_inner.inst = res.m_inst;
    }

    // 1 <= xd <= 31
    void xreg_write(XRegIdx xd) {
        assert(m_inner.valid);
        m_inner.xreg_mask |= 1 << xd.idx;
    }

    // 0 <= fd <= 31
    void freg_write(FRegIdx fd) {
        assert(m_inner.valid);
        m_inner.freg_mask |= 1 << fd.idx;
    }

    void vreg_write_multiple(uint32_t vd_mask) {
        assert(m_inner.valid);
        m_inner.freg_mask |= vd_mask;
    }

    void csr_write(uint16_t csr) {
        assert(m_inner.valid);

        assert(m_inner.csr_count < POKEDEX_MAX_CSR_WRITE);
        m_inner.csr_indices[m_inner.csr_count++] = csr;
    }

    void mstatus_write() {
        csr_write(0x300); // the idx of mstatus
    }
};
