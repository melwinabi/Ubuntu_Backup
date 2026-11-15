import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { db } from "@/lib/db";
import CreditScoreClient from "@/components/credit-score/CreditScoreClient";

export const revalidate = 0;

export default async function CreditScorePage() {
  const session = await getServerSession(authOptions);
  const userId = session!.user!.id;

  const history = await db.creditScore.findMany({
    where: { userId },
    orderBy: { recordedOn: "asc" },
  });

  return (
    <CreditScoreClient
      initialHistory={history}
    />
  );
}
