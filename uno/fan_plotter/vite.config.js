import { defineConfig } from 'vite'

export default defineConfig({
  server: {
    fs: {
      // Allow serving sibling folders for previewing mockup assets
      allow: ['d:/HVL2025/ADA525/uno', 'C:/Users/carlos/Documents/PlatformIO/Projects/motor test']
    }
  }
})