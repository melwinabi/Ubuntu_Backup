import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { db } from "@/lib/db";
import SettingsClient from "@/components/settings/SettingsClient";

export const revalidate = 0;

export default async function SettingsPage() {
  const session = await getServerSession(authOptions);
  const userId = session!.user!.id;

  const accounts = await db.account.findMany({
    where: { userId, isActive: true },
    orderBy: { accountName: "asc" },
  });

  const dbUser = await db.user.findUnique({
    where: { id: userId },
    select: {
      id: true,
      customId: true,
      fullName: true,
      email: true,
      dob: true,
      profilePic: true,
    },
  });

  return (
    <SettingsClient
      initialAccounts={accounts}
      user={dbUser}
    />
  );
}
