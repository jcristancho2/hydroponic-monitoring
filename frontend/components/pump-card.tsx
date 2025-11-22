import { Card } from "@/components/ui/card";
import { Power } from "lucide-react";
import { getDatabase, ref, set } from "firebase/database";

interface PumpCardProps {
  title: string;
  status: boolean;
  description?: string;
  bombaKey: string; // "bomba_agua", "bomba_sustrato", "bomba_solucion"
}

export function PumpCard({ title, status, description, bombaKey }: PumpCardProps) {
  const handlePowerClick = () => {
    const db = getDatabase();
    set(ref(db, `/hydroponic_data/control/${bombaKey}`), !status);
  };

  return (
    <Card className="border-border/50 bg-card/50 p-6 backdrop-blur transition-all hover:border-border">
      <div className="flex items-center justify-between">
        <div className="flex items-center gap-3">
          <div
            className={`rounded-lg p-2.5 ${
              status
                ? "bg-green-500/10 text-green-500"
                : "bg-muted text-muted-foreground"
            }`}
            style={{ cursor: "pointer" }}
            onClick={handlePowerClick}
            title="Encender/Apagar bomba"
          >
            <Power className="h-5 w-5" />
          </div>
          <div>
            <p className="font-medium">{title}</p>
            <p className="text-sm text-muted-foreground">
              {description || (status ? "Activa" : "Inactiva")}
            </p>
          </div>
        </div>
        <div className="flex items-center gap-2">
          <div
            className={`h-3 w-3 rounded-full ${
              status ? "bg-green-500 animate-pulse" : "bg-muted"
            }`}
          />
          <span
            className={`text-sm font-medium ${
              status ? "text-green-500" : "text-muted-foreground"
            }`}
          >
            {status ? "ON" : "OFF"}
          </span>
        </div>
      </div>
    </Card>
  );
}

<div className="grid gap-3 sm:gap-4 md:grid-cols-3 lg:gap-6 mt-4">
  <div>
    <button
      className="px-4 py-2 bg-green-600 text-white rounded mr-2"
      onClick={() => setBombaCommand("bomba_agua", 1)}
    >
      Encender Bomba Agua
    </button>
    <button
      className="px-4 py-2 bg-red-600 text-white rounded"
      onClick={() => setBombaCommand("bomba_agua", 0)}
    >
      Apagar Bomba Agua
    </button>
  </div>
  <div>
    <button
      className="px-4 py-2 bg-green-600 text-white rounded mr-2"
      onClick={() => setBombaCommand("bomba_sustrato", 1)}
    >
      Encender Bomba pH-
    </button>
    <button
      className="px-4 py-2 bg-red-600 text-white rounded"
      onClick={() => setBombaCommand("bomba_sustrato", 0)}
    >
      Apagar Bomba pH-
    </button>
  </div>
  <div>
    <button
      className="px-4 py-2 bg-green-600 text-white rounded mr-2"
      onClick={() => setBombaCommand("bomba_solucion", 1)}
    >
      Encender Bomba pH+
    </button>
    <button
      className="px-4 py-2 bg-red-600 text-white rounded"
      onClick={() => setBombaCommand("bomba_solucion", 0)}
    >
      Apagar Bomba pH+
    </button>
  </div>
</div>
