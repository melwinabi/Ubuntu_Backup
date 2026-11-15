import { MetadataRoute } from 'next'
 
export default function manifest(): MetadataRoute.Manifest {
  return {
    name: 'Smart Relay',
    short_name: 'Smart Relay',
    description: 'High-performance control interface for Smart Relay Switches',
    start_url: '/',
    display: 'standalone',
    background_color: '#1a1c23',
    theme_color: '#1a1c23',
    icons: [
      {
        src: '/icon.svg',
        sizes: '192x192 512x512 any',
        type: 'image/svg+xml',
      }
    ],
  }
}
