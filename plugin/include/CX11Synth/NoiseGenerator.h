#pragma once


//namespace audio_plugin {
    class NoiseGenerator {
        public:
            void reset() {
                noise_seed = 22222;
            }

            float next_value() {
                noise_seed = noise_seed * 196314165 + 907633515;
                int temp = int(noise_seed >> 7) - 16777216;
                return float(temp) / 16777216.0f;
            }

        private:
            unsigned int noise_seed;
    };
//} // End namespace