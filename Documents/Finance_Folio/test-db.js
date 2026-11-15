const { PrismaClient } = require('@prisma/client');
const prisma = new PrismaClient();

async function main() {
  const users = await prisma.user.findMany();
  for (let i = 0; i < users.length; i++) {
    const customId = `FGAT${String(i + 1).padStart(4, '0')}`;
    await prisma.user.update({
      where: { id: users[i].id },
      data: { customId }
    });
    console.log(`Updated user ${users[i].email} to ${customId}`);
  }
}

main().catch(console.error).finally(() => prisma.$disconnect());
