import { 
  createRootRoute, 
  createRoute, 
  createRouter,
  Outlet
} from '@tanstack/react-router';
import { MainLayout } from '../components/layouts/MainLayout';
import { HomePage } from '../components/pages/HomePage';
import { ZenPage } from '../components/pages/ZenPage';

const rootRoute = createRootRoute({
  component: MainLayout
});

const indexRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: '/',
  component: HomePage
});

const zenRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: '/zen',
  component: ZenPage
});

const routeTree = rootRoute.addChildren([indexRoute, zenRoute]);

export const router = createRouter({ routeTree });

declare module '@tanstack/router' {
  interface Register {
    router: typeof router;
  }
}
