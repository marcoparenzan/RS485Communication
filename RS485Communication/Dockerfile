FROM microsoft/dotnet:2.1-runtime AS base
WORKDIR /app

FROM microsoft/dotnet:2.1-sdk AS build
WORKDIR /src
COPY RS485Communication/RS485Communication.csproj RS485Communication/
RUN dotnet restore RS485Communication/RS485Communication.csproj
COPY . .
WORKDIR /src/RS485Communication
RUN dotnet build RS485Communication.csproj -c Release -o /app

FROM build AS publish
RUN dotnet publish RS485Communication.csproj -c Release -o /app

FROM base AS final
WORKDIR /app
COPY --from=publish /app .
ENTRYPOINT ["dotnet", "RS485Communication.dll"]
