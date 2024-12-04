import { NextResponse } from "next/server";
import { createWriteStream, unlink } from "fs";
import path from "path";
import ffmpeg from "fluent-ffmpeg";
import { Readable } from "stream";

export const config = { api: { bodyParser: false } };

export async function POST(req: Request) {
  try {
    // Lê o corpo da requisição como um buffer de imagem
    const chunks: Uint8Array[] = [];
    const reader = req.body?.getReader();

    if (reader) {
      // Coleta os dados da requisição como um array de buffers
      let done = false;
      while (!done) {
        const { value, done: readerDone } = await reader.read();
        if (value) {
          chunks.push(value);
        }
        done = readerDone;
      }
    }

    // Converte o array de buffers para um único buffer
    const buffer = Buffer.concat(chunks);

    if (!buffer) {
      return NextResponse.json({ error: "Nenhum arquivo enviado" }, { status: 400 });
    }

    // Cria o diretório temporário na raiz do projeto, caso não exista
    const tempDir = path.join(process.cwd(), "temp");
    const tempPath = path.join(tempDir, `${Date.now()}.jpg`);

    // Cria o diretório se não existir
    const fs = require("fs");
    if (!fs.existsSync(tempDir)) {
      fs.mkdirSync(tempDir);
    }

    // Salvar o arquivo recebido temporariamente
    const tempStream = createWriteStream(tempPath);
    const readable = Readable.from(buffer);
    readable.pipe(tempStream);

    // Após salvar, use o FFmpeg para gerar HLS
    const outputDir = path.join(process.cwd(), "public/hls");
    if (!fs.existsSync(outputDir)) {
      fs.mkdirSync(outputDir, { recursive: true });
    }
    const outputFile = path.join(outputDir, "stream.m3u8");

    ffmpeg(tempPath)
      .inputFormat("image2pipe")
      .outputOptions([
        "-preset ultrafast",                // Configuração de codificação rápida
        "-hls_time 2",                      // Define a duração de cada segmento (em segundos)
        "-hls_playlist_type event",         // Define a playlist como tipo 'event' (para eventos ao vivo)
        "-r 30",                            // Define o FPS (frames por segundo) como 30
        "-hls_list_size 0",                 // Não limita o número de entradas na playlist .m3u8
        "-hls_flags independent_segments"
      ])
      .output(outputFile)
      .on("end", () => {

        // Excluir o arquivo temporário após o processamento
        unlink(tempPath, (err) => {
          if (err) {
            console.error("Erro ao excluir o arquivo temporário", err);
          }
        });
      })
      .run();

    return NextResponse.json({ message: "Frame processado" });
  } catch (err) {
    console.error(err);
    return NextResponse.json({ error: "Erro no processamento" });
  }
}
