/** @type {import('postcss-load-config').Config} */
const config = {
  plugins: {
    // Usa el nombre que el error te solicita.
    '@tailwindcss/postcss': {}, 
    'autoprefixer': {},
  },
}

export default config