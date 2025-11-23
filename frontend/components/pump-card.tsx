import { Card } from "@/components/ui/card";
import { Power } from "lucide-react";

interface PumpCardProps {
  title: string;
  status: boolean;
  description?: string;
  bombaKey: string; // "bomba_agua", "bomba_sustrato", "bomba_solucion"
}

export function PumpCard({ title, status, description, bombaKey }: PumpCardProps) {
  return (
    <Card className="border-border/50 bg-card/50 p-6 backdrop-blur transition-all hover:border-border">
      <div className="flex flex-col items-center justify-center text-center">
        <div
          className={`rounded-lg p-2.5 mb-3 ${
            status
              ? "bg-green-500/10 text-green-500"
              : "bg-muted text-muted-foreground"
          }`}
        >
          <Power className="h-5 w-5" />
        </div>
        <p className="font-medium mb-1">{title}</p>
        <p className="text-sm text-muted-foreground mb-3">
          {description || (status ? "Activa" : "Inactiva")}
        </p>
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
