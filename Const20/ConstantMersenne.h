#pragma once

namespace constant
{
    namespace dist
    {
        template <class _Ty, int _Wx, int _Nx, int _Mx, int _Rx, _Ty _Px, int _Ux, int _Sx, _Ty _Bx, int _Tx, _Ty _Cx, int _Lx>
        class const_mersenne_twister : public std::_Circ_buf<_Ty, _Nx>
        { 
            // mersenne twister generator
        public:
            using result_type = _Ty;

            static constexpr int word_size = _Wx;
            static constexpr int state_size = _Nx;
            static constexpr int shift_size = _Mx;
            static constexpr int mask_bits = _Rx;
            static constexpr _Ty parameter_a = _Px;
            static constexpr int output_u = _Ux;
            static constexpr int output_s = _Sx;
            static constexpr _Ty output_b = _Bx;
            static constexpr int output_t = _Tx;
            static constexpr _Ty output_c = _Cx;
            static constexpr int output_l = _Lx;

            static constexpr _Ty default_seed = 5489U;

            consteval const_mersenne_twister() : _Dxval(_WMSK)
            {
                seed(default_seed, static_cast<_Ty>(1812433253));
            }

            consteval explicit const_mersenne_twister(_Ty _Xx0, _Ty _Dxarg = _WMSK, _Ty _Fxarg = static_cast<_Ty>(1812433253)) : _Dxval(_Dxarg) 
            {
                seed(_Xx0, _Fxarg);
            }

            template <class _Gen, std::_Enable_if_seed_seq_t<_Gen, const_mersenne_twister> = 0>
            consteval explicit const_mersenne_twister(_Gen& _Gx) : _Dxval(_WMSK) 
            {
                seed(_Gx);
            }

            consteval void seed(_Ty _Xx0 = default_seed, _Ty _Fx = static_cast<_Ty>(1812433253)) 
            {
                // set initial values from specified value
                _Ty _Prev = this->_Ax[0] = _Xx0 & _WMSK;
                for (size_t _Ix = 1; _Ix < _Nx; ++_Ix) {
                    _Prev = this->_Ax[_Ix] = (_Ix + _Fx * (_Prev ^ (_Prev >> (_Wx - 2)))) & _WMSK;
                }

                this->_Idx = _Nx;
            }

            template <class _Gen, std::_Enable_if_seed_seq_t<_Gen, const_mersenne_twister> = 0>
            consteval void seed(_Gen& _Gx, bool = false) 
            { 
                // set initial values from range
                for (size_t _Ix = 0; _Ix < _Nx; ++_Ix) {
                    this->_Ax[_Ix] = _Gx() & _WMSK;
                }

                this->_Idx = _Nx;
            }

            consteval result_type(min)() const
            {
                return 0;
            }

            consteval result_type(max)() const
            {
                return _WMSK;
            }

            consteval result_type operator()()
            {
                if (this->_Idx == _Nx) {
                    _Refill_upper();
                }
                else if (2 * _Nx <= this->_Idx) {
                    _Refill_lower();
                }

                _Ty _Res = this->_Ax[this->_Idx++] & _WMSK;
                _Res ^= (_Res >> _Ux) & _Dxval;
                _Res ^= (_Res << _Sx) & _Bx;
                _Res ^= (_Res << _Tx) & _Cx;
                _Res ^= (_Res & _WMSK) >> _Lx;
                return _Res;
            }

            consteval void discard(unsigned long long _Nskip) 
            { 
                // discard _Nskip elements
                for (; 0 < _Nskip; --_Nskip) {
                    (void)(*this)();
                }
            }

        protected:
            consteval void _Refill_lower()
            { 
                // compute values for the lower half of the history array
                constexpr size_t _Wrap_bound_one = _Nx - _One_mod_n;
                constexpr size_t _Wrap_bound_m = _Nx - _M_mod_n;

                if constexpr (_M_mod_n == 0) {
                    for (size_t _Ix = 0; _Ix < _Wrap_bound_one; ++_Ix) 
                    { 
                        // fill in values
                        const _Ty _Tmp = (this->_Ax[_Ix + _Nx] & _HMSK) | (this->_Ax[_Ix + _Nx + _One_mod_n] & _LMSK);
                        this->_Ax[_Ix] = (_Tmp >> 1) ^ (_Tmp & 1 ? _Px : 0) ^ this->_Ax[_Ix + _Nx + _M_mod_n];
                    }

                    if constexpr (_One_mod_n == 1)
                    { 
                        // fill in _Ax[_Nx - 1]
                        constexpr size_t _Ix = _Wrap_bound_one;

                        const _Ty _Tmp = (this->_Ax[_Ix + _Nx] & _HMSK) | (this->_Ax[_Ix - _Nx + _One_mod_n] & _LMSK);
                        this->_Ax[_Ix] = (_Tmp >> 1) ^ (_Tmp & 1 ? _Px : 0) ^ this->_Ax[_Ix + _Nx + _M_mod_n];
                    }
                }
                else 
                {
                    for (size_t _Ix = 0; _Ix < _Wrap_bound_m; ++_Ix) 
                    { 
                        // fill in lower region
                        const _Ty _Tmp = (this->_Ax[_Ix + _Nx] & _HMSK) | (this->_Ax[_Ix + _Nx + _One_mod_n] & _LMSK);
                        this->_Ax[_Ix] = (_Tmp >> 1) ^ (_Tmp & 1 ? _Px : 0) ^ this->_Ax[_Ix + _Nx + _M_mod_n];
                    }

                    for (size_t _Ix = _Wrap_bound_m; _Ix < _Wrap_bound_one; ++_Ix) 
                    {
                        // fill in upper region (avoids modulus operation)
                        const _Ty _Tmp = (this->_Ax[_Ix + _Nx] & _HMSK) | (this->_Ax[_Ix + _Nx + _One_mod_n] & _LMSK);
                        this->_Ax[_Ix] = (_Tmp >> 1) ^ (_Tmp & 1 ? _Px : 0) ^ this->_Ax[_Ix - _Nx + _M_mod_n];
                    }

                    if constexpr (_One_mod_n == 1) 
                    { 
                        // fill in _Ax[_Nx - 1]
                        constexpr size_t _Ix = _Wrap_bound_one;

                        const _Ty _Tmp = (this->_Ax[_Ix + _Nx] & _HMSK) | (this->_Ax[_Ix - _Nx + _One_mod_n] & _LMSK);
                        this->_Ax[_Ix] = (_Tmp >> 1) ^ (_Tmp & 1 ? _Px : 0) ^ this->_Ax[_Ix - _Nx + _M_mod_n];
                    }
                }

                this->_Idx = 0;
            }

            consteval void _Refill_upper() 
            { 
                // compute values for the upper half of the history array
                for (size_t _Ix = _Nx; _Ix < 2 * _Nx; ++_Ix) 
                { 
                    // fill in values
                    const _Ty _Tmp = (this->_Ax[_Ix - _Nx] & _HMSK) | (this->_Ax[_Ix - _Nx + _One_mod_n] & _LMSK);
                    this->_Ax[_Ix] = (_Tmp >> 1) ^ (_Tmp & 1 ? _Px : 0) ^ this->_Ax[_Ix - _Nx + _M_mod_n];
                }
            }

            _Ty _Dxval;

            static constexpr _Ty _WMSK = ~((~_Ty{ 0 } << (_Wx - 1)) << 1);
            static constexpr _Ty _HMSK = (_WMSK << _Rx) & _WMSK;
            static constexpr _Ty _LMSK = ~_HMSK & _WMSK;

            static constexpr int _One_mod_n = 1 % _Nx; // either 0 or 1
            static constexpr int _M_mod_n = _Mx % _Nx;
        };


        template <class _Ty, size_t _Wx, size_t _Nx, size_t _Mx, size_t _Rx, _Ty _Px, size_t _Ux, _Ty _Dx, size_t _Sx, _Ty _Bx,
            size_t _Tx, _Ty _Cx, size_t _Lx, _Ty _Fx>
        class const_mersenne_twister_engine : public const_mersenne_twister<_Ty, _Wx, _Nx, _Mx, _Rx, _Px, _Ux, _Sx, _Bx, _Tx, _Cx, _Lx>
        {
        public:
            static constexpr unsigned long long _Max = (((1ULL << (_Wx - 1)) - 1) << 1) + 1;

            static_assert(0 < _Mx && _Mx <= _Nx && 2U < _Wx && _Rx <= _Wx && _Ux <= _Wx && _Sx <= _Wx && _Tx <= _Wx
                && _Lx <= _Wx && _Wx <= std::numeric_limits<_Ty>::digits && _Px <= _Max && _Bx <= _Max && _Cx <= _Max
                && _Dx <= _Max && _Fx <= _Max,
                "invalid template argument for mersenne_twister_engine");

            using _Mybase = const_mersenne_twister<_Ty, _Wx, _Nx, _Mx, _Rx, _Px, _Ux, _Sx, _Bx, _Tx, _Cx, _Lx>;
            using result_type = _Ty;

            static constexpr size_t word_size = _Wx;
            static constexpr size_t state_size = _Nx;
            static constexpr size_t shift_size = _Mx;
            static constexpr size_t mask_bits = _Rx;
            static constexpr _Ty xor_mask = _Px;
            static constexpr size_t tempering_u = _Ux;
            static constexpr _Ty tempering_d = _Dx;
            static constexpr size_t tempering_s = _Sx;
            static constexpr _Ty tempering_b = _Bx;
            static constexpr size_t tempering_t = _Tx;
            static constexpr _Ty tempering_c = _Cx;
            static constexpr size_t tempering_l = _Lx;
            static constexpr _Ty initialization_multiplier = _Fx;

            static constexpr result_type default_seed = 5489U;

            const_mersenne_twister_engine() : _Mybase(default_seed, _Dx, _Fx) {}

            consteval explicit const_mersenne_twister_engine(result_type _Xx0) : _Mybase(_Xx0, _Dx, _Fx) {}

            template <class _Seed_seq, std::_Enable_if_seed_seq_t<_Seed_seq, const_mersenne_twister_engine> = 0>
            consteval explicit const_mersenne_twister_engine(_Seed_seq& _Seq) : _Mybase(default_seed, _Dx, _Fx)
            {
                seed(_Seq);
            }

            consteval void seed(result_type _Xx0 = default_seed)
            {
                // set initial values from specified value
                _Mybase::seed(_Xx0, _Fx);
            }

            template <class _Seed_seq, std::_Enable_if_seed_seq_t<_Seed_seq, const_mersenne_twister_engine> = 0>
            consteval void seed(_Seed_seq& _Seq)
            {
                // reset sequence from seed sequence
                constexpr int _Kx = (_Wx + 31) / 32;
                unsigned long _Arr[_Kx * _Nx];
                _Seq.generate(&_Arr[0], &_Arr[_Kx * _Nx]);

                int _Idx0 = 0;
                _Ty _Sum = 0;
                for (size_t _Ix = 0; _Ix < _Nx; ++_Ix, _Idx0 += _Kx)
                {
                    // pack _Kx words
                    this->_Ax[_Ix] = static_cast<_Ty>(_Arr[_Idx0]);

                    for (int _Jx = 1; _Jx < _Kx; ++_Jx)
                    {
                        this->_Ax[_Ix] |= static_cast<_Ty>(_Arr[_Idx0 + _Jx]) << (32 * _Jx);
                    }

                    this->_Ax[_Ix] &= this->_WMSK;

                    if (_Ix == 0)
                    {
                        _Sum = this->_Ax[_Ix] >> _Rx;
                    }
                    else {
                        _Sum |= this->_Ax[_Ix];
                    }
                }

                if (_Sum == 0)
                {
                    this->_Ax[0] = _Ty{ 1 } << (_Wx - 1);
                }

                this->_Idx = _Nx;
            }

            static consteval result_type(min)()
            {
                return 0;
            }

            static consteval result_type(max)()
            {
                return _Mybase::_WMSK;
            }
        };
    }
}