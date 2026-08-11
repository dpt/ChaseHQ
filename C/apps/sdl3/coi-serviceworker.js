/*! coi-serviceworker v0.1.7 - Guido Zuidhof, licensed under MIT */
/* Vendored from https://github.com/gzuraan/coi-serviceworker so the pthread
 * build works on GitHub Pages, which cannot send the
 * Cross-Origin-Opener-Policy / Cross-Origin-Embedder-Policy response headers
 * SharedArrayBuffer (and so Emscripten's -pthread) requires -- this service
 * worker intercepts navigation/fetch and injects them client-side instead.
 */
let coepCredentialless = false;
if (typeof window === 'undefined') {
  self.addEventListener("install", () => self.skipWaiting());
  self.addEventListener("activate", (event) => event.waitUntil(self.clients.claim()));

  self.addEventListener("message", (ev) => {
    if (!ev.data) {
      return;
    } else if (ev.data.type === "deregister") {
      self.registration
        .unregister()
        .then(() => {
          return self.clients.matchAll();
        })
        .then((clients) => {
          clients.forEach((client) => client.navigate(client.url));
        });
    } else if (ev.data.type === "coepCredentialless") {
      coepCredentialless = ev.data.value;
    }
  });

  self.addEventListener("fetch", function (event) {
    const r = event.request;
    if (r.cache === "only-if-cached" && r.mode !== "same-origin") {
      return;
    }

    const request = (coepCredentialless && r.mode === "no-cors")
      ? new Request(r, {
        credentials: "omit",
      })
      : r;
    event.respondWith(
      fetch(request)
        .then((response) => {
          if (response.status === 0) {
            return response;
          }

          const newHeaders = new Headers(response.headers);
          newHeaders.set("Cross-Origin-Embedder-Policy",
            coepCredentialless ? "credentialless" : "require-corp"
          );
          if (!coepCredentialless) {
            newHeaders.set("Cross-Origin-Resource-Policy", "cross-origin");
          }
          newHeaders.set("Cross-Origin-Opener-Policy", "same-origin");

          return new Response(response.body, {
            status: response.status,
            statusText: response.statusText,
            headers: newHeaders,
          });
        })
        .catch((e) => console.error(e))
    );
  });
} else {
  (() => {
    const reloadedBySelf = window.sessionStorage.getItem("coiReloadedBySelf");
    window.sessionStorage.removeItem("coiReloadedBySelf");
    const coepDegrading = (reloadedBySelf == "coepdegrade");

    // You can customize the behavior of this script through a global `coi` variable.
    const coi = {
      shouldRegister: () => true,
      shouldDeregister: () => false,
      coepCredentialless: () => true,
      coepDegrade: () => true,
      doReload: () => window.location.reload(),
      quiet: false,
      ...window.coi
    };

    const n = navigator;

    if (n.serviceWorker && n.serviceWorker.controller) {
      n.serviceWorker.controller.postMessage({
        type: "coepCredentialless",
        value: coepDegrading ? false : coi.coepCredentialless(),
      });

      if (coi.shouldDeregister()) {
        n.serviceWorker.controller.postMessage({ type: "deregister" });
      }
    }

    // If we're already coi: do nothing. Perhaps it's due to this script doing its job, or COOP/COEP are
    // already set from the origin server. Also if the browser has no notion of crossOriginIsolated, just give up here.
    if (window.crossOriginIsolated !== false || !coi.shouldRegister()) return;

    if (!window.isSecureContext) {
      !coi.quiet && console.log("COOP/COEP Service Worker not registered, a secure context is required.");
      return;
    }

    // In some environments (e.g. Chrome incognito mode) this won't be available
    if (n.serviceWorker) {
      n.serviceWorker.register(window.document.currentScript.src).then(
        (registration) => {
          !coi.quiet && console.log("COOP/COEP Service Worker registered", registration.scope);

          registration.addEventListener("updatefound", () => {
            !coi.quiet && console.log("Reloading page to make use of updated COOP/COEP Service Worker.");
            window.sessionStorage.setItem("coiReloadedBySelf", "updatefound");
            window.location.reload();
          });

          // If the registration is active, but it's not controlling the page
          if (registration.active && !n.serviceWorker.controller) {
            !coi.quiet && console.log("Reloading page to make use of COOP/COEP Service Worker.");
            window.sessionStorage.setItem("coiReloadedBySelf", "notcontrolling");
            window.location.reload();
          }
        },
        (err) => {
          if (!coepDegrading && coi.coepDegrade()) {
            !coi.quiet && console.log("COOP/COEP Service Worker failed to register, trying credentialless COEP", err);
            window.sessionStorage.setItem("coiReloadedBySelf", "coepdegrade");
            window.location.reload();
          } else {
            !coi.quiet && console.error("COOP/COEP Service Worker failed to register:", err);
          }
        }
      );
    }
  })();
}
