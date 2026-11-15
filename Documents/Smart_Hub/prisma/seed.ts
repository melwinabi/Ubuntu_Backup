import { PrismaClient } from '@prisma/client'

const prisma = new PrismaClient()

async function main() {
  console.log('Seeding initial series and sub-series data...')

  // Create W1_Series
  const w1Series = await prisma.deviceSeries.upsert({
    where: { seriesName: 'W1_Series' },
    update: {},
    create: {
      seriesName: 'W1_Series',
    },
  })

  const emails = ["asmelwin3@gmail.com", "admin@gmail.com"]

  for (const email of emails) {
    await prisma.adminEmail.upsert({
      where: { email },
      update: {},
      create: { email },
    })
  }

  const subHub = await prisma.deviceSubSeries.upsert({
    where: { id: 1 },
    update: {},
    create: {
      id: 1,
      seriesId: w1Series.id,
      subSeriesName: 'W1_Smart_Hub',
    },
  })

  await prisma.deviceSubSeries.upsert({
    where: { id: 2 },
    update: {},
    create: {
      id: 2,
      seriesId: w1Series.id,
      subSeriesName: 'W1_Switch',
    },
  })

  // Create a Sample Device for testing
  await prisma.device.upsert({
    where: { id: 101 },
    update: {},
    create: {
      id: 101,
      seriesId: w1Series.id,
      subSeriesId: subHub.id,
      deviceName: 'Living Room Hub',
      serverId: 'srv_001',
      mqttTopic: 'w1/hub/101/main',
      status: 'Online',
      widgets: {
        create: [
          { widgetName: 'Temperature', mqttTopic: 'w1/hub/101/temp' },
          { widgetName: 'Humidity', mqttTopic: 'w1/hub/101/hum' }
        ]
      }
    }
  })

  // Cleanup garbage data
  await prisma.device.deleteMany({
    where: {
      deviceName: {
        contains: 'xcxcxc'
      }
    }
  })

  console.log('Seeding completed successfully.')
}

main()
  .catch((e) => {
    console.error(e)
    process.exit(1)
  })
  .finally(async () => {
    await prisma.$disconnect()
  })
