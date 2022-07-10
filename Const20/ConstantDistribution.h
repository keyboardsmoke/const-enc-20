#pragma once

namespace constant
{
    namespace detail
    {
        template <class _Real, size_t _Bits, class _Gen>
        consteval _Real const_generate_canonical(_Gen& _Gx)
        {
            // build a floating-point value from random sequence
            constexpr auto _Digits = static_cast<size_t>(std::numeric_limits<_Real>::digits);
            constexpr auto _Minbits = static_cast<int>(_Digits < _Bits ? _Digits : _Bits);

            constexpr auto _Gxmin = static_cast<_Real>((_Gen::min)());
            constexpr auto _Gxmax = static_cast<_Real>((_Gen::max)());
            constexpr auto _Rx = (_Gxmax - _Gxmin) + _Real{ 1 };

            constexpr int _Kx = std::_Generate_canonical_iterations(_Minbits, (_Gen::min)(), (_Gen::max)());

            _Real _Ans{ 0 };
            _Real _Factor{ 1 };

            for (int _Idx = 0; _Idx < _Kx; ++_Idx) { // add in another set of bits
                _Ans += (static_cast<_Real>(_Gx()) - _Gxmin) * _Factor;
                _Factor *= _Rx;
            }

            return _Ans / _Factor;
        }

        template <class _Real, class _Gen>
        consteval _Real const_Nrand_impl(_Gen& _Gx)
        {
            // build a floating-point value from random sequence
            constexpr auto _Digits = static_cast<size_t>(std::numeric_limits<_Real>::digits);
            constexpr auto _Bits = ~size_t{ 0 };
            constexpr auto _Minbits = _Digits < _Bits ? _Digits : _Bits;

            if constexpr (std::_Has_static_min_max<_Gen>::value && _Minbits <= 64) {
                return const_generate_canonical<_Real, _Minbits>(_Gx);
            }
            else { // TRANSITION, for tr1 machinery only; Standard machinery can call generate_canonical directly
                const _Real _Gxmin = static_cast<_Real>((_Gx.min)());
                const _Real _Gxmax = static_cast<_Real>((_Gx.max)());
                const _Real _Rx = (_Gxmax - _Gxmin) + _Real{ 1 };

                const int _Ceil = static_cast<int>(_STD ceil(static_cast<_Real>(_Minbits) / _STD log2(_Rx)));
                const int _Kx = _Ceil < 1 ? 1 : _Ceil;

                _Real _Ans{ 0 };
                _Real _Factor{ 1 };

                for (int _Idx = 0; _Idx < _Kx; ++_Idx) { // add in another set of bits
                    _Ans += (static_cast<_Real>(_Gx()) - _Gxmin) * _Factor;
                    _Factor *= _Rx;
                }

                return _Ans / _Factor;
            }
        }

        template <class _Diff, class _Urng>
        class const_Rng_from_urng 
        { 
            // wrap a URNG as an RNG
        public:
            using _Ty0 = std::make_unsigned_t<_Diff>;
            using _Ty1 = typename _Urng::result_type;

            using _Udiff = std::conditional_t<sizeof(_Ty1) < sizeof(_Ty0), _Ty0, _Ty1>;

            consteval explicit const_Rng_from_urng(_Urng& _Func) : _Ref(_Func), _Bits(CHAR_BIT * sizeof(_Udiff)), _Bmask(_Udiff(-1)) {
                for (; (_Urng::max)() - (_Urng::min)() < _Bmask; _Bmask >>= 1) {
                    --_Bits;
                }
            }

            consteval _Diff operator()(_Diff _Index) { // adapt _Urng closed range to [0, _Index)
                for (;;) { // try a sample random value
                    _Udiff _Ret = 0; // random bits
                    _Udiff _Mask = 0; // 2^N - 1, _Ret is within [0, _Mask]

                    while (_Mask < _Udiff(_Index - 1)) { // need more random bits
                        _Ret <<= _Bits - 1; // avoid full shift
                        _Ret <<= 1;
                        _Ret |= _Get_bits();
                        _Mask <<= _Bits - 1; // avoid full shift
                        _Mask <<= 1;
                        _Mask |= _Bmask;
                    }

                    // _Ret is [0, _Mask], _Index - 1 <= _Mask, return if unbiased
                    if (_Ret / _Index < _Mask / _Index || _Mask % _Index == _Udiff(_Index - 1)) {
                        return static_cast<_Diff>(_Ret % _Index);
                    }
                }
            }

            consteval _Udiff _Get_all_bits() {
                _Udiff _Ret = 0;

                for (size_t _Num = 0; _Num < CHAR_BIT * sizeof(_Udiff); _Num += _Bits) { // don't mask away any bits
                    _Ret <<= _Bits - 1; // avoid full shift
                    _Ret <<= 1;
                    _Ret |= _Get_bits();
                }

                return _Ret;
            }

            const_Rng_from_urng(const const_Rng_from_urng&) = delete;
            const_Rng_from_urng& operator=(const const_Rng_from_urng&) = delete;

        private:
            consteval _Udiff _Get_bits() { // return a random value within [0, _Bmask]
                for (;;) { // repeat until random value is in range
                    _Udiff _Val = _Ref() - (_Urng::min)();

                    if (_Val <= _Bmask) {
                        return _Val;
                    }
                }
            }

            _Urng& _Ref; // reference to URNG
            size_t _Bits; // number of random bits generated by _Get_bits()
            _Udiff _Bmask; // 2^_Bits - 1
        };
    }

    template <class _Ty = int>
    class const_uniform_int { // uniform integer distribution
    public:
        using result_type = _Ty;

        struct param_type 
        { 
            // parameter package
            using distribution_type = const_uniform_int;

            consteval param_type() {
                _Init(0, 9);
            }

            consteval explicit param_type(result_type _Min0, result_type _Max0 = 9) {
                _Init(_Min0, _Max0);
            }

            consteval result_type a() const {
                return _Min;
            }

            consteval result_type b() const {
                return _Max;
            }

            consteval void _Init(_Ty _Min0, _Ty _Max0) 
            { 
                // set internal state
                // _STL_ASSERT(_Min0 <= _Max0, "invalid min and max arguments for uniform_int");
                _Min = _Min0;
                _Max = _Max0;
            }

            result_type _Min;
            result_type _Max;
        };

        consteval const_uniform_int() : _Par(0, 9) {}

        consteval explicit const_uniform_int(_Ty _Min0, _Ty _Max0 = 9) : _Par(_Min0, _Max0) {}

        consteval explicit const_uniform_int(const param_type& _Par0) : _Par(_Par0) {}

        consteval result_type a() const {
            return _Par.a();
        }

        consteval result_type b() const {
            return _Par.b();
        }

        consteval param_type param() const {
            return _Par;
        }

        consteval void param(const param_type& _Par0) { // set parameter package
            _Par = _Par0;
        }

        consteval result_type(min)() const {
            return _Par._Min;
        }

        consteval result_type(max)() const {
            return _Par._Max;
        }

        consteval void reset() {} // clear internal state

        template <class _Engine>
        consteval result_type operator()(_Engine& _Eng) const {
            return _Eval(_Eng, _Par._Min, _Par._Max);
        }

        template <class _Engine>
        consteval result_type operator()(_Engine& _Eng, const param_type& _Par0) const {
            return _Eval(_Eng, _Par0._Min, _Par0._Max);
        }

        template <class _Engine>
        consteval result_type operator()(_Engine& _Eng, result_type _Nx) const {
            return _Eval(_Eng, 0, _Nx - 1);
        }

    private:
        using _Uty = std::make_unsigned_t<_Ty>;

        template <class _Engine>
        consteval result_type _Eval(_Engine& _Eng, _Ty _Min, _Ty _Max) const 
        { 
            // compute next value in range [_Min, _Max]
            detail::const_Rng_from_urng<_Uty, _Engine> _Generator(_Eng);

            const _Uty _Umin = _Adjust(static_cast<_Uty>(_Min));
            const _Uty _Umax = _Adjust(static_cast<_Uty>(_Max));

            _Uty _Uret;

            if (_Umax - _Umin == static_cast<_Uty>(-1)) 
            {
                _Uret = static_cast<_Uty>(_Generator._Get_all_bits());
            }
            else 
            {
                _Uret = static_cast<_Uty>(_Generator(static_cast<_Uty>(_Umax - _Umin + 1)));
            }

            return static_cast<_Ty>(_Adjust(static_cast<_Uty>(_Uret + _Umin)));
        }

        consteval static _Uty _Adjust(_Uty _Uval) 
        { 
            // convert signed ranges to unsigned ranges and vice versa
            if constexpr (std::is_signed_v<_Ty>) 
            {
                const _Uty _Adjuster = (static_cast<_Uty>(-1) >> 1) + 1; // 2^(N-1)

                if (_Uval < _Adjuster) 
                {
                    return static_cast<_Uty>(_Uval + _Adjuster);
                }
                else 
                {
                    return static_cast<_Uty>(_Uval - _Adjuster);
                }
            }
            else 
            { 
                // _Ty is already unsigned, do nothing
                return _Uval;
            }
        }

        param_type _Par;
    };

    template <class _Ty = int>
    class const_uniform_int_distribution : public const_uniform_int<_Ty> { // uniform integer distribution
    public:
        using _Mybase = const_uniform_int<_Ty>;
        using _Mypbase = typename _Mybase::param_type;
        using result_type = typename _Mybase::result_type;

        struct param_type : _Mypbase { // parameter package
            using distribution_type = const_uniform_int_distribution;

            consteval param_type() : _Mypbase(0, (std::numeric_limits<_Ty>::max)()) {}
            consteval explicit param_type(result_type _Min0, result_type _Max0 = (std::numeric_limits<_Ty>::max)()) : _Mypbase(_Min0, _Max0) {}
            consteval param_type(const _Mypbase& _Right) : _Mypbase(_Right) {}
        };

        consteval const_uniform_int_distribution() : _Mybase(0, (std::numeric_limits<_Ty>::max)()) {}

        consteval explicit const_uniform_int_distribution(_Ty _Min0, _Ty _Max0 = (std::numeric_limits<_Ty>::max)()) : _Mybase(_Min0, _Max0) {}

        consteval explicit const_uniform_int_distribution(const param_type& _Par0) : _Mybase(_Par0) {}
    };

    template <class _Ty = double>
    class const_uniform_real { // uniform real distribution
    public:
        using result_type = _Ty;

        struct param_type { // parameter package
            using distribution_type = const_uniform_real;

            consteval param_type() {
                _Init(_Ty{ 0 }, _Ty{ 1 });
            }

            consteval explicit param_type(_Ty _Min0, _Ty _Max0 = _Ty{ 1 }) {
                _Init(_Min0, _Max0);
            }

            consteval result_type a() const {
                return _Min;
            }

            consteval result_type b() const {
                return _Max;
            }

            consteval void _Init(_Ty _Min0, _Ty _Max0) { // set internal state
                _STL_ASSERT(_Min0 <= _Max0 && (0 <= _Min0 || _Max0 <= _Min0 + (numeric_limits<_Ty>::max)()),
                    "invalid min and max arguments for uniform_real");
                _Min = _Min0;
                _Max = _Max0;
            }

            result_type _Min;
            result_type _Max;
        };

        consteval const_uniform_real() : _Par(_Ty{ 0 }, _Ty{ 1 }) {}

        consteval explicit const_uniform_real(_Ty _Min0, _Ty _Max0 = _Ty{ 1 }) : _Par(_Min0, _Max0) {}

        consteval explicit const_uniform_real(const param_type& _Par0) : _Par(_Par0) {}

        consteval result_type a() const {
            return _Par.a();
        }

        consteval result_type b() const {
            return _Par.b();
        }

        consteval param_type param() const {
            return _Par;
        }

        consteval void param(const param_type& _Par0) { // set parameter package
            _Par = _Par0;
        }

        consteval result_type(min)() const {
            return _Par._Min;
        }

        consteval result_type(max)() const {
            return _Par._Max;
        }

        consteval void reset() {} // clear internal state

        template <class _Engine>
        consteval result_type operator()(_Engine& _Eng) const {
            return _Eval(_Eng, _Par);
        }

        template <class _Engine>
        consteval result_type operator()(_Engine& _Eng, const param_type& _Par0) const {
            return _Eval(_Eng, _Par0);
        }

    private:
        template <class _Engine>
        consteval result_type _Eval(_Engine& _Eng, const param_type& _Par0) const {
            return (detail::const_Nrand_impl<_Ty>(_Eng)) * (_Par0._Max - _Par0._Min) + _Par0._Min;
        }

        param_type _Par;
    };

    template <class _Ty = double>
    class const_uniform_real_distribution : public const_uniform_real<_Ty> { // uniform real distribution
    public:
        using _Mybase = const_uniform_real<_Ty>;
        using _Mypbase = typename _Mybase::param_type;
        using result_type = typename _Mybase::result_type;

        struct param_type : _Mypbase { // parameter package
            using distribution_type = const_uniform_real_distribution;

            consteval param_type() : _Mypbase(_Ty{ 0 }, _Ty{ 1 }) {}

            consteval explicit param_type(_Ty _Min0, _Ty _Max0 = _Ty{ 1 }) : _Mypbase(_Min0, _Max0) {}

            consteval param_type(const _Mypbase& _Right) : _Mypbase(_Right) {}
        };

        consteval const_uniform_real_distribution() : _Mybase(_Ty{ 0 }, _Ty{ 1 }) {}

        consteval explicit const_uniform_real_distribution(_Ty _Min0, _Ty _Max0 = _Ty{ 1 }) : _Mybase(_Min0, _Max0) {}

        consteval explicit const_uniform_real_distribution(const param_type& _Par0) : _Mybase(_Par0) {}
    };
}